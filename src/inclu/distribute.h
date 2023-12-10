#pragma once

#include "../PCH.h"



class Distributor {
    public:
        static void distributeKeywordsToTomes() {

            if (!Settings::SETTINGS.bOnlySpellsWithTomes) {
                return;
            }
            
            bool ignoreNoPerkSpells = Settings::SETTINGS.bExcludeNoPerkSpells;

            for (auto * book : RE::TESDataHandler::GetSingleton()->GetFormArray<RE::TESObjectBOOK>()) {
                
                auto * spell = book->GetSpell();
                if (!spell) { return; }
                if (!ignoreNoPerkSpells
                    && (!spell->data.castingPerk
                    || spell->data.castingPerk->formID == 0x0)
                ) {
                    logger::info(" -- curr spell EDID {} is excluded.", spell->GetFormEditorID());
                }
                bool added = spell->AddKeyword(Consts::KYWD_ITEM_APPLIED_SPELL);
                
                logger::info("Adding inclusion keyword to tome-taught spell EDID = {}, success? {}",
                    spell->GetFormEditorID(),
                    (added) ? "yes" : "no"
                );
            }
        }

};