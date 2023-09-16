extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
{
#ifndef DEBUG
	auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
	auto path = logger::log_directory();
	if (!path) {
		return false;
	}

	*path /= Version::PROJECT;
	*path += ".log"sv;
	auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));

#ifndef DEBUG
	log->set_level(spdlog::level::trace);
#else
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);
#endif

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("%g(%#): [%^%l%$] %v"s);

	logger::info(FMT_STRING("{} v{}"), Version::PROJECT, Version::NAME);

	a_info->infoVersion = SKSE::PluginInfo::kVersion;
	a_info->name = Version::PROJECT.data();
	a_info->version = Version::MAJOR;

	if (a_skse->IsEditor()) {
		logger::critical("Loaded in editor, marking as incompatible"sv);
		return false;
	}

	const auto ver = a_skse->RuntimeVersion();
	if (ver < SKSE::RUNTIME_1_5_39) {
		logger::critical(FMT_STRING("Unsupported runtime version {}"), ver.string());
		return false;
	}

	return true;
}

struct Settings
{
private:
	using iSetting = AutoTOML::iSetting;
	using bSetting = AutoTOML::bSetting;
	using fSetting = AutoTOML::fSetting;
	using sSetting = AutoTOML::sSetting;

public:
	static void load()
	{
		try {
			const auto table = toml::parse_file("Data/SKSE/Plugins/ConfigurableCastTime.toml"s);
			for (const auto& setting : AutoTOML::ISetting::get_settings()) {
				setting->load(table);
			}
		} catch (const toml::parse_error& e) {
			std::ostringstream ss;
			ss
				<< "Error parsing file \'" << *e.source().path << "\':\n"
				<< '\t' << e.description() << '\n'
				<< "\t\t(" << e.source().begin << ')';
			logger::error(ss.str());
			throw std::runtime_error("failed to load settings"s);
		}
	}

	static inline bSetting EnableNPC{ "General"s, "EnableNPC"s, false };
	static inline bSetting EnablePlayer{ "General"s, "EnablePlayer"s, true };

	//static inline iSetting Formid{ "General"s, "FormID"s, 0x800 };
	//static inline sSetting EspName{ "General"s, "Esp"s, "ConfigurableCastTime.esp" };
};

//struct DataHandler
//{
//	static inline RE::TESGlobal* var = nullptr;
//
//	static void init() {
//		var = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(static_cast<uint32_t>(*Settings::Formid), *Settings::EspName);
//	}
//};

float get_skill_pers(RE::Actor* a, RE::SpellItem* spel) {
	const float MAX_SKILL = 100.0f;
	auto av = spel->GetAssociatedSkill();
	return av != RE::ActorValue::kNone ? a->GetActorValue(av) / MAX_SKILL : -1.0f;
}

float get_cast_time_Impl([[maybe_unused]] float origin, [[maybe_unused]] RE::Actor* caster, [[maybe_unused]] RE::SpellItem* spel)
{
	//return origin * DataHandler::var->value;
	
	//37.8255 x^(5)-74.8895 x^(4)+43.3082 x^(3)-7.65238 x^(2)+4.82019 x-0.411975
	
	float x = get_skill_pers(caster, spel);
	if (x < 0.0f)
		return origin;

	if (x >= 1.0f)
		return 0.0f;

	return origin / (37.8255f * x * x * x * x * x - 74.8895f * x * x * x * x + 43.3082f * x * x * x - 7.65238f * x * x + 4.82019f * x - 0.411975f);
}

bool is_playable_spel(RE::SpellItem* spel) {
	using ST = RE::MagicSystem::SpellType;
	using AV = RE::ActorValue;

	auto type = spel->GetSpellType();
	if (type == ST::kStaffEnchantment || type == ST::kScroll || type == ST::kSpell || type == ST::kLeveledSpell) {
		auto av = spel->GetAssociatedSkill();
		return av == AV::kAlteration || av == AV::kConjuration || av == AV::kDestruction || av == AV::kIllusion ||
		       av == AV::kRestoration;
	}

	return false;
}

float get_cast_time(float origin, RE::Actor* a, RE::SpellItem* spel)
{
	bool isPlayer = a->IsPlayerRef();
	if (is_playable_spel(spel) && (isPlayer && *Settings::EnablePlayer || !isPlayer && *Settings::EnableNPC)) {
		return get_cast_time_Impl(origin, a, spel);
	} else {
		return origin;
	}
}

float get_cast_time_caster(float origin, RE::MagicCaster* _a, RE::SpellItem* spel)
{
	auto a = _a->GetCasterAsActor();
	if (!a)
		return origin;
	
	return get_cast_time(origin, a, spel);
}

float get_cast_time_addone(float origin, RE::Character* a, RE::SpellItem* spel) { return get_cast_time(origin, a, spel) + 1.0f; }

float get_cast_time_spell_caster(RE::SpellItem* spel, RE::MagicCaster* _a)
{
	return get_cast_time_caster(spel->GetChargeTime(), _a, spel);
}

template <bool eq>
bool MagicItem__is_continious_common(RE::SpellItem* spel, RE::MagicCaster* caster)
{
	if (!spel)
		return false;

	using namespace RE::MagicSystem;
	auto type = spel->GetSpellType();
	if (type != SpellType::kSpell && type != SpellType::kLesserPower && type != SpellType::kPoison &&
		(type != SpellType::kEnchantment && type != SpellType::kStaffEnchantment ||
			spel->GetCastingType() == CastingType::kConstantEffect || caster->GetCastingSource() == CastingSource::kInstant))
		return false;

	float charge_time = get_cast_time_spell_caster(spel, caster);
	if (eq) {
		if (charge_time == 0.0)
			return false;
	} else {
		if (charge_time != 0.0)
			return false;
	}

	return spel->GetCastingType() == CastingType::kFireAndForget;
}

bool MagicItem__is_continious_v1(RE::SpellItem* spel, RE::MagicCaster* caster)
{
	return MagicItem__is_continious_common<false>(spel, caster);
}

bool MagicItem__is_continious_v2(RE::SpellItem* spel, RE::MagicCaster* caster)
{
	return MagicItem__is_continious_common<true>(spel, caster);
}

void apply_hooks()
{
	using FenixUtils::add_trampoline;

	{
		// SkyrimSE.exe+54291A
		FenixUtils::writebytes<33364, 0x16a>("\x48\x89\xDA\x0F\x1F\x00"sv);  // rbx
		SKSE::GetTrampoline().write_call<5>(REL::ID(33364).address() + 0x173, uintptr_t(MagicItem__is_continious_v1));

		// SkyrimSE.exe+5425AF
		FenixUtils::writebytes<33363, 0x13f>("\x48\x89\xFA\x0F\x1F\x00"sv);  // rdi
		SKSE::GetTrampoline().write_call<5>(REL::ID(33363).address() + 0x148, uintptr_t(MagicItem__is_continious_v1));

		// SkyrimSE.exe+541DA5
		FenixUtils::writebytes<33359, 0x15>("\x48\x89\xCA\x0F\x1F\x00"sv);  // rcx
		SKSE::GetTrampoline().write_call<5>(REL::ID(33359).address() + 0x1e, uintptr_t(MagicItem__is_continious_v2));

		// SkyrimSE.exe+542198
		FenixUtils::writebytes<33362, 0x1d8>("\x48\x89\xDA\x0F\x1F\x00"sv);  // rcx
		SKSE::GetTrampoline().write_call<5>(REL::ID(33362).address() + 0x1e1, uintptr_t(MagicItem__is_continious_v2));
	}

	{  // ActorMagicCaster::Update
		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t func_addr, uintptr_t ret_addr)
			{
				// rcx == SpellItem*
				// rbx == MagicCaster*
				mov(rdx, rbx);
				mov(rax, func_addr);
				call(rax);

				mov(rdx, ret_addr);
				jmp(rdx);
			}
		} xbyakCode{ uintptr_t(get_cast_time_spell_caster), REL::ID(33362).address() + 0x21c };  // SkyrimSE.exe+5421DC
		add_trampoline<5, 33362, 0x216>(&xbyakCode);                                             // SkyrimSE.exe+5421D6
	}

	{  // PlayerCharacter::sub_1407BEED0
		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t func_addr, uintptr_t ret_addr)
			{
				// rdi == Character*
				// rbx == SpellItem*
				mov(r8, rbx);
				mov(rdx, rdi);
				mov(rax, func_addr);
				call(rax);

				mov(rdx, ret_addr);
				jmp(rdx);
			}
		} xbyakCode{ uintptr_t(get_cast_time_addone), REL::ID(46070).address() + 0x13d };  // SkyrimSE.exe+7BF00D
		add_trampoline<5, 46070, 0x135>(&xbyakCode);                                       // SkyrimSE.exe+7BF005
	}

	{  // MagicCaster::SetCastingTimerForCharge
		struct Code : Xbyak::CodeGenerator
		{
			Code(uintptr_t func_addr, uintptr_t ret_addr)
			{
				// rcx == SpellItem*
				// rbx == MagicCaster*
				mov(rdx, rbx);
				mov(rax, func_addr);
				call(rax);

				mov(rdx, ret_addr);
				jmp(rdx);
			}
		} xbyakCode{ uintptr_t(get_cast_time_spell_caster), REL::ID(33651).address() + 0x1b };  // SkyrimSE.exe+54F22B
		add_trampoline<5, 33651, 0x15>(&xbyakCode);                                             // SkyrimSE.exe+54F225
	}

	// ignored 140892130 (314 or 314/s, comparing with 0)
}

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Settings::load();
		//DataHandler::init();
		apply_hooks();

		break;
	}
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
{
	auto g_messaging = reinterpret_cast<SKSE::MessagingInterface*>(a_skse->QueryInterface(SKSE::LoadInterface::kMessaging));
	if (!g_messaging) {
		logger::critical("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}

	logger::info("loaded");

	SKSE::Init(a_skse);
	SKSE::AllocTrampoline(1 << 10);

	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);

	return true;
}
