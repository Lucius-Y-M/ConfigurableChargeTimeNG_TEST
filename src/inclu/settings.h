#pragma once


#include "../PCH.h"



namespace Settings {


    struct CastSpeedSetting {

        bool bAppliesTo_Player = false;
        bool bAppliesTo_NPC = false;


        u32 uMaxCastSpeedMinLevel_Novice;
        u32 uMaxCastSpeedMinLevel_Apprentice;
        u32 uMaxCastSpeedMinLevel_Adept;
        u32 uMaxCastSpeedMinLevel_Expert;
        u32 uMaxCastSpeedMinLevel_Master;
    };


    struct CastFailureSetting {

        /// FAIL:
        bool bCastCanFail_Player = false;
        bool bCastCanFail_NPC = false;

        u32 uNoFailMinLevel_Novice;
        u32 uNoFailMinLevel_Apprentice;
        u32 uNoFailMinLevel_Adept;
        u32 uNoFailMinLevel_Expert;
        u32 uNoFailMinLevel_Master;


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

