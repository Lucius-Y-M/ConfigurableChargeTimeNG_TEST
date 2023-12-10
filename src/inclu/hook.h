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
                    (actor->IsPlayerRef() && Settings::SETTINGS.sSpeed.bAppliesTo_Player)
                    || (Settings::SETTINGS.sSpeed.bAppliesTo_NPC && !actor->HasKeyword(Consts::KYWD_NPC_EXCLUDED))
                )
            ) {

                //// FAILS: then interrupt
                //// !!!!!!! ================== MUST TEST FIRST!
                if (Conditions::isCastFail(actor, spell)) {

                    // actor->InterruptCast(false);

                    // return _Update(caster, time);
                }
                //// !!!!!!! ================== MUST TEST FIRST!


                auto state = caster->state.underlying();
                                
                if (state == static_cast<u32>(MaCa::State::kUnk01) /* start */
                    || state == static_cast<u32>(MaCa::State::kUnk02) /* charge */
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
        static float getNewCastingTime(float originalTime, Actor * actor, Spell * spell) {
            return 1.f;
        }
    
};