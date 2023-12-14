#pragma once

#include "../PCH.h"







namespace Consts {

    using u32 = uint32_t;

    enum SpellLevel {
        
        kNovice,

        kApprent,
        kAdept,
        kExprt,
        kMastr,

        kUndefined
    };
    static_assert(static_cast<u32> ( SpellLevel::kUndefined ) == 5);


    static HashMap<const char *, SpellLevel> PERKS_FOR_SPELLS {

        // ===== NOVICE

        {"AlterationNovice00", SpellLevel::kNovice },
        {"ConjurationNovice00", SpellLevel::kNovice },
        {"DestructionNovice00", SpellLevel::kNovice },
        {"IllusionNovice00", SpellLevel::kNovice },
        {"RestorationNovice00", SpellLevel::kNovice },


        // ===== ALL OTHERS
        {"AlterationApprentice25", SpellLevel::kApprent },
        {"AlterationAdept50", SpellLevel::kAdept },
        {"AlterationExpert75", SpellLevel::kExprt },
        {"AlterationMaster100", SpellLevel::kMastr },

        {"ConjurationApprentice25", SpellLevel::kApprent },
        {"ConjurationAdept50", SpellLevel::kAdept },
        {"ConjurationExpert75", SpellLevel::kExprt },
        {"ConjurationMaster100", SpellLevel::kMastr },

        {"DestructionApprentice25", SpellLevel::kApprent },
        {"DestructionAdept50", SpellLevel::kAdept },
        {"DestructionExpert75", SpellLevel::kExprt },
        {"DestructionMaster100", SpellLevel::kMastr },

        {"IllusionApprentice25", SpellLevel:: kApprent },
        {"IllusionAdept50", SpellLevel::kAdept },
        {"IllusionExpert75", SpellLevel::kExprt },
        {"IllusionMaster100", SpellLevel::kMastr },

        {"RestorationApprentice25", SpellLevel:: kApprent},
        {"RestorationAdept50", SpellLevel::kAdept },
        {"RestorationExpert75", SpellLevel::kExprt },
        {"RestorationMaster100", SpellLevel::kMastr }
    };


    // ==== Key Const Values
    static const char * FILE_NAME = "ConfigurableMagicCasting.esp";


    //// =============== FIDs
    static const FID KYWD_ITEM_AFFECTED_SPELL_H = 0x800;
    static const FID KYWD_NPC_EXCLUDED_H = 0x801;


    static const FID SOND_CASTFAIL_H = 0x802;
    static const FID SPEL_BACKFIRE_H = 0x803;


    //// =============== Loadenda
    static Sound * SOND_CASTFAIL = nullptr;
    static Spell * SPEL_BACKFIRE = nullptr;


    static Keyword * KYWD_ITEM_APPLIED_SPELL = nullptr;
    static Keyword * KYWD_NPC_EXCLUDED = nullptr;





    // static std::unordered_set<Spell *> PLAYBLE_SPELLS;

    




    static const SpellLevel isSpellPerked(FID spellPerkFID);



    /*
        this should be called

    */
    static const Some<AV> isAcceptableSpellType(Spell * spell);


}