#pragma once


#include "../PCH.h"


using MaCa = RE::MagicCaster;

class CastSpeedHook {



    //// CREDIT: Fenix31415 (did all of the original hooking & conditions setup here; I only added the "READY" part)
    public:
	    static void Hook() { _Update = REL::Relocation<uintptr_t>(RE::VTABLE_ActorMagicCaster[0]).write_vfunc(0x1d, Update); }

    private:
        static void Update(MaCa * caster, float time) {

            //// CHECKS: caster is actor, spell exists, is playable, all keywords fit
            auto * actor = caster->GetCasterAsActor();
            if (!actor || caster->currentSpell) { return _Update(caster, time); }
            
            if (auto * spell = caster->currentSpell->As<Spell>();
                spell
                && spell->HasKeyword(Consts::KYWD_ITEM_APPLIED_SPELL)
                &&
                (
                    (actor->IsPlayerRef() & Settings::SETTINGS.sSpeed.bAppliesTo_Player)
                    | (Settings::SETTINGS.sSpeed.bAppliesTo_NPC & !actor->HasKeyword(Consts::KYWD_NPC_EXCLUDED))
                )
            ) {

                
                auto castRes = Conditions::isCastFail(actor, spell);
                switch (castRes) {

                    //// FAILS: then interrupt
                    //// !!!!!!! ================== MUST TEST FIRST!
                    // case Conditions::CastFailStatus::kFail : {

                    //     actor->InterruptCast(false); // do NOT restore magicka

                    //     return _Update(caster, time); //// still needed?
                    //     break;
                    // }

                    // case Conditions::CastFailStatus::kFailBackfire : {

                    //     actor->InterruptCast(false); // do NOT restore magicka
                        
                    //     if (auto * castInst = actor->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
                    //         castInst
                    //     ) {

                    //         float magnitudeOverride = 1.f;

                    //         castInst->CastSpellImmediate(Consts::SPEL_BACKFIRE, false, actor->AsReference(), 1.f, false, magnitudeOverride, nullptr);
                    //     }

                    //     return _Update(caster, time); //// still needed?
                    //     break;
                    // }
                    //// !!!!!!! ================== MUST TEST FIRST!




                    /* curr: ONLY success, so allow it to go ahead. */
                    default : {

                        break;
                    }

                }



                auto state = caster->state.underlying();
                                
                if (state == static_cast<u32>(MaCa::State::kUnk01) /* start */
                    || state == static_cast<u32>(MaCa::State::kUnk02) /* charge */
                    || state == static_cast<u32>(MaCa::State::kCasting) /* casting */
                ) {
                    
                    float tOrig = spell->GetChargeTime();
                    float tNew = getNewCastingTime(tOrig, actor, spell);

                    
                    //// TODO: MUST CHECK AGAIM
                    tNew = (tNew > 0.00001f) ? tOrig / tNew : 1000000.0f; 

	    			return _Update(caster, tNew);

                } else if (state == static_cast<u32>(MaCa::State::kCasting)) {

                }

            }

            return _Update(caster, time);

        }

        static inline REL::Relocation<decltype(Update)> _Update;





        //// CALCULATIONS
        static inline Consts::SpellLevel getSpellLevel(Spell * spell) {

            auto perkFID = spell->data.castingPerk->GetFormID();
            if ( Consts::PERKS_FOR_SPELLS.contains(perkFID) ) {
                return Consts::PERKS_FOR_SPELLS.at(perkFID);
            }
            return Consts::SpellLevel::kUndefined;
            
        }


        static float getNewCastingTime([[maybe_unused]] float originalTime, Actor * actor, Spell * spell) {
            
            Consts::SpellLevel level = getSpellLevel(spell);            
            if ( level == Consts::SpellLevel::kUndefined ) { return originalTime; }
            
            auto spellSchool = spell->GetAssociatedSkill();
            
            
            u32 skill = std::min(
                Settings::SETTINGS.uMaxSkillLevel,
                static_cast<u32>( actor->AsActorValueOwner()->GetActorValue(spellSchool) )
            );

            auto coeffL = Settings::SETTINGS.sSpeed.dCoefficientL;

            // save 2 attaseconds
            if (coeffL == 0.f) {
                return Settings::SETTINGS.sSpeed.SPEED_BASE_VALUES[skill];
            }

            return
                Settings::SETTINGS.sSpeed.SPEED_BASE_VALUES[skill]
                /
                (1 + static_cast<u32> ( level ) * Settings::SETTINGS.sSpeed.dCoefficientL )
            ;
        }
    
};