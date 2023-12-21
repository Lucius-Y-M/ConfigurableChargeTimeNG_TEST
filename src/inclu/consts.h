#pragma once

#include "../PCH.h"


#define UPPER_EFFECT_LIM 5  // should only have this many spell levels in game




namespace Consts {


        enum SpellLevel {
            
            kNovice,

            kApprent,
            kAdept,
            kExprt,
            kMastr,

            kUndefined
        };
        static_assert(static_cast<u32> ( SpellLevel::kUndefined ) == UPPER_EFFECT_LIM );


        static inline HashMap<FID, SpellLevel> PERKS_FOR_SPELLS {

            /*
                I originally just wanted to use EDID (instead of hardcoded FormIDs)
                from Skyrim.esm, but then discovered
                that some very popular mods (such as Ordinator, and - if my guess is right - Adamant as well)
                will CHANGE the EDIDs of vanilla records they overwrite
            

                This means FormIDs will *HAVE* to be used

                Here's hoping Bethesda never RENUBMERS any of the FormIDs, or we're all screwed        
            */

            //// ===== NOVICE

            { 0x000F2CAA, SpellLevel::kNovice },
            { 0x000F2CA9, SpellLevel::kNovice },
            { 0x000F2CA8, SpellLevel::kNovice },
            { 0x000F2CA7, SpellLevel::kNovice },
            { 0x000F2CA6, SpellLevel::kNovice },


            //// ===== ALL OTHERS

            // ALTER
            { 0x000C44B7, SpellLevel::kApprent },
            { 0x000C44B8, SpellLevel::kAdept },
            { 0x000C44B9, SpellLevel::kExprt },
            { 0x000C44BA, SpellLevel::kMastr },

            // CONJU
            { 0x000C44BB, SpellLevel::kApprent },
            { 0x000C44BC, SpellLevel::kAdept },
            { 0x000C44BD, SpellLevel::kExprt },
            { 0x000C44BE, SpellLevel::kMastr },

            // DESTR
            { 0x000C44BF, SpellLevel::kApprent },
            { 0x000C44C0, SpellLevel::kAdept },
            { 0x000C44C1, SpellLevel::kExprt },
            { 0x000C44C2, SpellLevel::kMastr },
            
            // ILLUS
            { 0x000C44C3, SpellLevel::kApprent },
            { 0x000C44C4, SpellLevel::kAdept },
            { 0x000C44C5, SpellLevel::kExprt },
            { 0x000C44C6, SpellLevel::kMastr },

            // RESTO
            { 0x000C44C7, SpellLevel::kApprent },
            { 0x000C44C8, SpellLevel::kAdept },
            { 0x000C44C9, SpellLevel::kExprt },
            { 0x000C44CA, SpellLevel::kMastr }
        };


        // ==== Key Const Values
        static const char * PLUGIN_FILE_NAME = "ConfigurableMagicCasting.esp";


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

        static const Some<AV> isAcceptableSpellType(Spell * spell) {

            auto type = spell->GetSpellType();
            /*
                kSpell = 0,
                kLeveledSpell = 9,
                kStaffEnchantment = 12,
                kScroll = 13
            */
            if (u32 kID = static_cast<u32>( type );
                (kID == 0)
                | (kID == 9)
                | (kID == 12)
                | (kID == 13)
            ) {

                auto av = spell->GetAssociatedSkill();
                u32 avID = static_cast <u32> ( av );

                /*
                    kAlteration = 18,
                    kConjuration = 19,
                    kDestruction = 20,
                    kIllusion = 21,
                    kRestoration = 22,
                */
                if (18 <= avID && avID <= 22) {

                    //// is correctly perked?
                    if ( !PERKS_FOR_SPELLS.contains(spell->data.castingPerk->GetFormID()) ) {
                        return None;
                    }

                    return Some(av);
                }
            } else {
                return None;
            }
        }


};