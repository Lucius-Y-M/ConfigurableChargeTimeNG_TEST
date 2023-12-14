#include "inclu/consts.h"


namespace Consts {



    /* this should ONLY be called if NOT restricted to tome-taught spells */
    // static Some<RE::ActorValue> isPlayableSpell(Spell * spell) {
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
                return Some(av);
            }
        } else {
            return None;
        }
    }

}