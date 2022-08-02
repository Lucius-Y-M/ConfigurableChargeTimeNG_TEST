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

	static inline iSetting Formid{ "General"s, "FormID"s, 0x800 };
	static inline sSetting EspName{ "General"s, "Esp"s, "ConfigurableCastTime.esp" };
	static inline bSetting EnableNPC{ "General"s, "EnableNPC"s, false };
	static inline bSetting EnablePlayer{ "General"s, "EnablePlayer"s, true };
};

struct DataHandler
{
	static inline RE::TESGlobal* var = nullptr;

	static void init() {
		var = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(static_cast<uint32_t>(*Settings::Formid), *Settings::EspName);
	}
};

float get_cast_time(float origin, RE::MagicCaster* _a)
{
	auto a = _a->GetCasterAsActor();
	if (!a)
		return origin;
	
	bool isPlayer = a->IsPlayerRef();
	if (isPlayer && *Settings::EnablePlayer || !isPlayer && *Settings::EnableNPC) {
		//static auto varr = RE::TESDataHandler::GetSingleton()->LookupForm<RE::TESGlobal>(static_cast<uint32_t>(*Settings::Formid), *Settings::EspName);
		return origin * DataHandler::var->value;
		//return origin * varr->value;
	} else {
		return origin;
	}
}

#include <xbyak\xbyak.h>
template <size_t BRANCH_TYPE, uint64_t ID, size_t offset = 0, bool call = false>
void add_trampoline(Xbyak::CodeGenerator* xbyakCode)
{
	constexpr REL::ID funcOffset = REL::ID(ID);
	auto funcAddr = funcOffset.address();
	auto size = xbyakCode->getSize();
	auto& trampoline = SKSE::GetTrampoline();
	auto result = trampoline.allocate(size);
	std::memcpy(result, xbyakCode->getCode(), size);
	if constexpr (!call)
		trampoline.write_branch<BRANCH_TYPE>(funcAddr + offset, (std::uintptr_t)result);
	else
		trampoline.write_call<BRANCH_TYPE>(funcAddr + offset, (std::uintptr_t)result);
}

void apply_hook() {
	// mul time to global
	
	// SkyrimSE.exe+54F231
	constexpr REL::ID funcOffset = REL::ID(33651);
	uintptr_t ret_addr = funcOffset.address() + 0x21;

	struct Code : Xbyak::CodeGenerator
	{
		Code(uintptr_t func_addr, uintptr_t ret_addr)
		{
			mov(rdx, rbx);
			mov(rax, func_addr);
			call(rax);

			// recover
			mov(rax, ptr[rbx]);
			mov(rcx, rbx);

			mov(rdx, ret_addr);
			jmp(rdx);
		}
	} xbyakCode{ uintptr_t(get_cast_time), ret_addr };
	add_trampoline<5, 33651, 0x1b>(&xbyakCode);  // SkyrimSE.exe+54F22B
}

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		Settings::load();
		DataHandler::init();
		apply_hook();

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
