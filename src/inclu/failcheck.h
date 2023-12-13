#pragma once

#include "../PCH.h"

#define UPPER_EFFECT_LIM 5


class Conditions {
    public:

        enum CastFailStatus {
            kSuccess,
            kFail,
            kFailBackfire
        };

        static CastFailStatus isCastFail(Actor * actor, Spell * spell) {

            //// NOTE: keyword etc. already checked, so just see if probability says should fail
            
                        

            return CastFailStatus::kSuccess;
        }

        static float calcMagnitudeOverride(Spell * spell) {

            /*
                get greatest magnitude;
                to prevent spells with 10+ (?!) effects,
                set an upper limit    
            */
            auto maxMagn = 0.f;
            u8 cnt = 0;
            for (auto * efct : spell->effects) {
                if (!efct) { continue; }

                if (efct->GetMagnitude() > maxMagn) {
                    maxMagn = efct->GetMagnitude();
                }

                cnt += 1;
                if (cnt == 5) {
                    break;
                }
            }

            return (maxMagn < Settings::SETTINGS.sFailureAndBackfire.fBackfireDefaultMinDamage) ? Settings::SETTINGS.sFailureAndBackfire.fBackfireDefaultMinDamage
                    : (maxMagn > Settings::SETTINGS.sFailureAndBackfire.fBackfireDefaultMaxDamage) ? Settings::SETTINGS.sFailureAndBackfire.fBackfireDefaultMaxDamage
                    : maxMagn
            ;
        }

    private:

        static CastFailStatus isFailBackfire(Actor * actor, Spell * spell) {


            return CastFailStatus::kFailBackfire;
        }


        static float CalculateChanceFail(Actor * actor, Spell * spell) {



            return 0.f;
        }

        static float CalculateChanceBackfire(Actor * actor, Spell * spell) {


            return 0.f;
        }


};