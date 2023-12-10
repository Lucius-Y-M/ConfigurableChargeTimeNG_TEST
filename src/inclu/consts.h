#pragma once

#include "../PCH.h"


namespace Consts {

    // ==== Key Const Values
    static const char * FILE_NAME = "ConfigurableMagicCasting.esp";


    static constexpr u32 PERKS_APPRE [1] = {0x810};
    static constexpr u32 PERKS_ADEPT [1] = {0x810};
    static constexpr u32 PERKS_EXPRT [1] = {0x810};
    static constexpr u32 PERKS_MASTR [1] = {0x810};



    //// =============== FIDs
    const FID KYWD_ITEM_AFFECTED_SPELL_H = 0x800;
    const FID KYWD_NPC_EXCLUDED_H = 0x801;


    const FID SOND_CASTFAIL_H = 0x802;
    const FID SPEL_BACKFIRE_H = 0x803;


    //// =============== Loadenda

    static Keyword * KYWD_ITEM_APPLIED_SPELL = nullptr;
    static Keyword * KYWD_NPC_EXCLUDED = nullptr;

    static Sound * SOND_CASTFAIL = nullptr;
    static Spell * SPEL_BACKFIRE = nullptr;



    // static std::unordered_set<Spell *> PLAYBLE_SPELLS;


}