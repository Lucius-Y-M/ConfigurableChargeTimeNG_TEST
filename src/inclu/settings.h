#pragma once


#include "../PCH.h"

#include <iostream>
#include <sstream>
#include <cmath>

template<typename T>
using Some = std::optional<T>;


using Polynomial = std::map<int, float>;



namespace Settings {

    //// ================================================================================================
    //// ================ FOR PARSING USER DEFINED FUNCTION

        static Some<Polynomial> parseInput(std::string input) {
            Polynomial polynomial;
            
            // Remove the surrounding brackets
            input = input.substr(1, input.size() - 2);

            // Create a string stream to parse the input
            std::istringstream iss(input);

            // Iterate over each pair in the input and parse it
            char ch;
            int key;
            float value;

            while (iss >> ch >> key >> ch >> value >> ch) {
                polynomial[key] = value;
            }

            return polynomial;
        }
        
        static float evaluatePolynomial(Polynomial & polynomial, int xVal) {
            float result = 0.f;

            // Iterate over the terms of the polynomial and accumulate the result
            for (const auto & term : polynomial) {
                int power = term.first;
                int coefficient = term.second;
                result += coefficient * std::pow(xVal, power);
            }

            return result;
        }
        //// generate PRE-CALCULATED values, for faster access later
        //// This will be called in LoadedListener
        template <std::ranges::view View>
        static constexpr auto generateSpeedValsVector(Polynomial & polynomial, View range) {

            std::vector<float> resultVector;
            resultVector.reserve(range.size());

            std::transform(
                range.begin(),
                range.end(),
                std::back_inserter(resultVector),
                [&polynomial] (int casterSkillLvl) {
                    return evaluatePolynomial(polynomial, casterSkillLvl);
                }
            );

            return resultVector;
        }

        //// ================ FOR PARSING USER DEFINED FUNCTION
        //// ================================================================================================



        struct CastSpeedSetting {

            bool bAppliesTo_Player = false;
            bool bAppliesTo_NPC = false;



            //// FORMULA:
            /*
                f(x, L) = (0.786696 + 0.0116971 x - 0.0000528884 x^2 + 9.13347×10^-8 x^3) / (1 + L * 0.3)
            */
            bool bCustomFormula = false;
            Polynomial l_idTermsOfXAndCoefficients = {
                {0, 0.786696},
                {1, 0.0116971},
                {2, -0.0000528884},
                {3, 9.13347f * pow(10, -8)},
            };
            float dCoefficientL = 0.25f;
            //// generate PRE-CALCULATED default values, for faster access later
            std::vector<float> SPEED_BASE_VALUES = generateSpeedValsVector( l_idTermsOfXAndCoefficients, std::views::iota(0, 100) );


            u32 uMaxCastSpeedMinLevel_Novice = 20;
            u32 uMaxCastSpeedMinLevel_Apprentice = 35;
            u32 uMaxCastSpeedMinLevel_Adept = 50;
            u32 uMaxCastSpeedMinLevel_Expert = 65;
            u32 uMaxCastSpeedMinLevel_Master = 85;
        };


        struct CastFailureSetting {

            //// FAIL:
            bool bCastCanFail_Player = false;
            bool bCastCanFail_NPC = false;

            u32 uNoFailMinLevel_Novice = 25;
            u32 uNoFailMinLevel_Apprentice = 45;
            u32 uNoFailMinLevel_Adept = 55;
            u32 uNoFailMinLevel_Expert = 75;
            u32 uNoFailMinLevel_Master = 95;

        };

        struct CastBackfireSetting {
            //// BACKFIRE:
            bool bFailCanBackfire_Player = true;
            bool bFailCanBackfire_NPC = true;


            //// FORMULA: BackfireChance = MAX ( MIN_CHANCE, L * SLCoeff + M * ( uNoFailMinLevel - uCasterSkillLevel ) )
            float fBackfireChanceMin = 0.3f;
            float fBackfireChanceMult = 0.4f;
            float fBackfireChanceSLCoeff = 0.1f;


            float fBackfireDefaultMinDamage = 100.0;
            float fBackfireDefaultMaxDamage = 1000.0;   // if set to -1.0, meaning NO UPPER LIMIT
        };


        struct MainSettings {
            u32 uMaxSkillLevel = 100;
            bool bOnlySpellsWithTomes = true;

            CastSpeedSetting sSpeed;
            CastFailureSetting sFailure;
            CastBackfireSetting sBackfire;

        };














        //// ============= SINGLETON
        static MainSettings SETTINGS;

};
