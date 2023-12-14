#include "PCH.h"





void OnDataLoadedHandler(SKSEMI::Message * msg) {
    if (msg->type == SKSEMI::kDataLoaded) {

        //// ===================== LOAD keywords
        auto * kywd_item = RE::TESDataHandler::GetSingleton()->LookupForm<Keyword>(Consts::KYWD_ITEM_AFFECTED_SPELL_H, Consts::PLUGIN_FILE_NAME);
        auto * kywd_npc_exclu = RE::TESDataHandler::GetSingleton()->LookupForm<Keyword>(Consts::KYWD_NPC_EXCLUDED_H, Consts::PLUGIN_FILE_NAME);

        if (!kywd_item || !kywd_npc_exclu) {
            logger::critical("INIT FAILURE 00; !!! DLL IS NOT LOADED AND WILL NOT WORK");
            logger::debug("Checking r1: {}", kywd_item ? "normal" : "not normal");
            logger::debug("Checking r2: {}", kywd_npc_exclu ? "normal" : "not normal");
            return;
        }

        Consts::KYWD_ITEM_APPLIED_SPELL = kywd_item;
        Consts::KYWD_NPC_EXCLUDED = kywd_npc_exclu;



        //// ===================== LOAD sound & fail spell
        auto * sond_castfail = RE::TESDataHandler::GetSingleton()->LookupForm<Sound>(Consts::SOND_CASTFAIL_H, Consts::PLUGIN_FILE_NAME);
        auto * spel_backfire = RE::TESDataHandler::GetSingleton()->LookupForm<Spell>(Consts::SPEL_BACKFIRE_H, Consts::PLUGIN_FILE_NAME);

        if (!sond_castfail || !spel_backfire) {
            logger::critical("INIT FAILURE 01; !!! DLL IS NOT LOADED AND WILL NOT WORK");
            logger::debug("Checking r1: {}", sond_castfail ? "normal" : "not normal");
            logger::debug("Checking r2: {}", spel_backfire ? "normal" : "not normal");
            return;
        }

        Consts::SOND_CASTFAIL = sond_castfail;
        Consts::SPEL_BACKFIRE = spel_backfire;
        
        //// ===================== TOME-TAUGHT SPELLS: Keyword distrib
        logger::info("== Tome-taught spells: starting keyword distribution, if required...");
        Distributor::distributeKeywordsToTomes();
        logger::info("== Tome-taught spells: keyword distribution complete, if executed.");
        
        //// ===================== HOOK
        logger::info("======= Parsing Settings INI...");
        auto [isSuccess, Msg] = Parser::parseINIFile();
        if ( !isSuccess ) {
            logger::warn(">>>>>>>>>>> WARNING: INI parsing failed in part / in full.");
            logger::warn(">>>>>>>>>>> The DLL will STILL WORK, but it may be using a mix of custom & default values,");
            logger::warn(">>>>>>>>>>> and may not behave exactly as expected.");
            logger::debug(">>>>>>>>>>> Parsing Failure detail: {}", Msg);
        } else {
            logger::info("======= Parsing Settings INI succesful.");
        }



        //// ===================== HOOK
        logger::info("== RE::ActorMagicCaster::Update Hook == installing...");
        CastSpeedHook::Hook();
        logger::info("== RE::ActorMagicCaster::Update Hook == installed.");

        logger::info("=========== INIT FINISHED. ===========");

    }
}

SKSEPluginLoad(const SKSE::LoadInterface *skse) {
    SKSE::Init(skse);

    // Once all plugins and mods are loaded, then the ~ console is ready and can
    // be printed to
    SKSE::GetMessagingInterface()->RegisterListener(OnDataLoadedHandler);
    
    return true;
}