#pragma once


#include "../PCH.h"



namespace Settings {


    struct CastSpeedSetting {

        bool bAppliesTo_Player = false;
        bool bAppliesTo_NPC = false;


        uint32_t uMaxCastSpeedMinLevel_Novice;
        uint32_t uMaxCastSpeedMinLevel_Apprentice;
        uint32_t uMaxCastSpeedMinLevel_Adept;
        uint32_t uMaxCastSpeedMinLevel_Expert;
        uint32_t uMaxCastSpeedMinLevel_Master;
    };


    struct CastFailureSetting {

        /// FAIL:
        bool bCastCanFail_Player = false;
        bool bCastCanFail_NPC = false;

        uint32_t uNoFailMinLevel_Novice;
        uint32_t uNoFailMinLevel_Apprentice;
        uint32_t uNoFailMinLevel_Adept;
        uint32_t uNoFailMinLevel_Expert;
        uint32_t uNoFailMinLevel_Master;


        /// BACKFIRE:
        bool bFailCanBackfire_Player;
        bool bFailCanBackfire_NPC;



        float fBackfireDefaultChanceMult;

        float fBackfireDefaultMaxDamage;
        float fBackfireDefaultMinDamage;
    };



    struct MainSettings {
        bool bOnlySpellsWithTomes = false;
        bool bExcludeNoPerkSpells = false;

        CastSpeedSetting sSpeed;
        CastFailureSetting sFailureAndBackfire;

    };


    static MainSettings SETTINGS;

}

