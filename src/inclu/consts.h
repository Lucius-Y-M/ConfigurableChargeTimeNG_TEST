#pragma once

#include "../PCH.h"







namespace Consts {

    using u32 = uint32_t;


    // ==== Key Const Values
    static const char * FILE_NAME = "ConfigurableMagicCasting.esp";

    static const HashSet<u32> PERKS_FOR_SPELLS {

        /*
            order: appren -> ... -> master
            

            This is how the FIDs will be checked against:

                idx % 4.

            in other words, idx == 3 -> 3 $ 4 = 3, corresp: kMaster (3).

            
        */

        //// ALTER:
        0x810,

        //// ILLUS:
        0x810,
        
        //// CONJU:
        0x810,

        //// DESTR:
        0x810,

        //// RESTO:
        0x810,

    };




    //// =============== FIDs
    static const FID KYWD_ITEM_AFFECTED_SPELL_H = 0x800;
    static const FID KYWD_NPC_EXCLUDED_H = 0x801;


    static const FID SOND_CASTFAIL_H = 0x802;
    static const FID SPEL_BACKFIRE_H = 0x803;


    //// =============== Loadenda
    static Sound * SOND_CASTFAIL = nullptr;
    static RE::SpellItem * SPEL_BACKFIRE = nullptr;


    static Keyword * KYWD_ITEM_APPLIED_SPELL = nullptr;
    static Keyword * KYWD_NPC_EXCLUDED = nullptr;





    // static std::unordered_set<Spell *> PLAYBLE_SPELLS;

    


    enum SpellLevel {
        
        kApprent,
        kAdept,
        kExprt,
        kMastr,

        kNoviceOrUndefined
    };
    static_assert(sizeof(SpellLevel) == 5 * sizeof(u32));

    static const SpellLevel isSpellPerked(FID spellPerkFID);



    /*
        this should be called

    */
    static const Some<RE::ActorValue> isAcceptableSpellType(RE::SpellItem * spell);



}