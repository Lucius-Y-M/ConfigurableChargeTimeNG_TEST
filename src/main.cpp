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

#define STB

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
			ss << "Error parsing file \'" << *e.source().path << "\':\n"
			   << '\t' << e.description() << '\n'
			   << "\t\t(" << e.source().begin << ')';
			logger::error(ss.str());
			throw std::runtime_error("failed to load settings"s);
		}
	}

	static inline bSetting EnableNPC{ "General"s, "EnableNPC"s, false };
	static inline bSetting EnablePlayer{ "General"s, "EnablePlayer"s, true };

#ifdef STB
	static inline iSetting Formid{ "General"s, "FormID"s, 0x800 };
	static inline sSetting EspName{ "General"s, "Esp"s, "ConfigurableCastTime.esp" };
#endif
};

#ifdef STB
struct DataHandler
{
	static inline RE::TESGlobal* var = nullptr;

	static void init() {
		var = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(static_cast<uint32_t>(*Settings::Formid), *Settings::EspName);
	}
};
#endif  // STB

float get_skill_pers(RE::Actor* a, RE::SpellItem* spel) {
	const float MAX_SKILL = 100.0f;
	auto av = spel->GetAssociatedSkill();
	return av != RE::ActorValue::kNone ? a->GetActorValue(av) / MAX_SKILL : -1.0f;
}

float get_cast_time_Impl([[maybe_unused]] float origin, [[maybe_unused]] RE::Actor* caster, [[maybe_unused]] RE::SpellItem* spel)
{
#ifdef STB
	return origin * DataHandler::var->value;
#else
	//37.8255 x^(5)-74.8895 x^(4)+43.3082 x^(3)-7.65238 x^(2)+4.82019 x-0.411975

	float x = get_skill_pers(caster, spel);
	if (x < 0.0f)
		return origin;

	if (x >= 1.0f)
		return 0.0f;

	return origin / (37.8255f * x * x * x * x * x - 74.8895f * x * x * x * x + 43.3082f * x * x * x - 7.65238f * x * x +
						4.82019f * x - 0.411975f);
#endif  // 0
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

class CastSpeedHook
{
public:
	static void Hook() { _Update = REL::Relocation<uintptr_t>(RE::VTABLE_ActorMagicCaster[0]).write_vfunc(0x1d, Update); }
	//static void Hook() { _Update = SKSE::GetTrampoline().write_branch<5>(REL::ID(33362).address() + 0x49e, Update); }

private:
	static void Update(RE::MagicCaster* mcaster, float dtime) {
		using S = RE::MagicCaster::State;
		auto state = mcaster->state.underlying();
		if (state == static_cast<uint32_t>(S::kUnk01) || state == static_cast<uint32_t>(S::kUnk02)) {
			if (auto a = mcaster->GetCasterAsActor();
				a && a->IsPlayerRef() && mcaster->currentSpell && mcaster->currentSpell->As<RE::SpellItem>()) {
				auto spel = mcaster->currentSpell->As<RE::SpellItem>();
				float time_origin = mcaster->currentSpell->GetChargeTime();
				float time_new = get_cast_time(time_origin, a, spel);
				float k = time_new > 0.00001f ? time_origin / time_new : 1000000.0f;
				return _Update(mcaster, dtime * k);
			}
		}
		return _Update(mcaster, dtime);
	}

	static inline REL::Relocation<decltype(Update)> _Update;
};

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Settings::load();
#ifdef STB
		DataHandler::init();
#endif  // STB

		CastSpeedHook::Hook();
		//apply_hooks();

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
