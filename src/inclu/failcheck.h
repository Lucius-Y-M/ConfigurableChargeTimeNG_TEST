#pragma once

#include "../PCH.h"




class Conditions {
    public:
        static bool isCastFail(Actor * actor, Spell * spell) {



            return false;
        }

        static bool isFailBackfire(Actor * actor, Spell * spell) {


            return false;
        }


    private:

        static float CalculateChanceFail(Actor * actor, Spell * spell) {



            return 0.f;
        }

        static float CalculateChanceBackfire(Actor * actor, Spell * spell) {


            return 0.f;
        }


};