#pragma once

#include "../PCH.h"
#include "../../inipp/inipp/inipp.h"

const char * FILE_NAME = "CMC_Settings.ini";

namespace Parser {



    static tup<bool, const char *> parseINIFile() {


        inipp::Ini<char> ini;
        try
        {
            std::ifstream is(FILE_NAME);
            ini.parse(is);
        }

        //// parsing failure
        catch(const std::exception& e)
        {
            return {false, "!! FAILED TO READ/OPEN SETTINGS INI FILE! PLEASE CHECK IF FILE NAME HAS BEEN CHANGED, OR IF ITS STRUCTURE HAS BEEN DAMAGED."};
        }


        ini.strip_trailing_comments();
        ini.interpolate();
        
        //// get all curr items
        try
        {
            int maxSkill = -1;
            inipp::get_value(ini.sections["General"], "uMaxSkillLevel", maxSkill);

            if (maxSkill > 100) {
                Settings::SETTINGS.uMaxSkillLevel = maxSkill;
            }

            inipp::get_value(ini.sections["General"], "bOnlySpellsWithTomes", Settings::SETTINGS.bOnlySpellsWithTomes);

            inipp::get_value(ini.sections["CastingSpeed"], "bAppliesTo_Player", Settings::SETTINGS.sSpeed.bAppliesTo_Player);
            inipp::get_value(ini.sections["CastingSpeed"], "bAppliesTo_NPC", Settings::SETTINGS.sSpeed.bAppliesTo_NPC);


            //// CUSTOMFORMULA:
            bool customFormula = false;
            inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "bCustomFormula", customFormula);
            if (customFormula) {
                std::string input;
                inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "l_idTermsOfXAndCoefficients", input);
                inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "dCoefficientL", Settings::SETTINGS.sSpeed.dCoefficientL);

                auto parseInput_opt = Settings::parseInput( input );
                
                if (parseInput_opt.has_value()) {
                    Settings::SETTINGS.sSpeed.l_idTermsOfXAndCoefficients = parseInput_opt.value();
                    Settings::SETTINGS.sSpeed.SPEED_BASE_VALUES = Settings::generateSpeedValsVector( std::views::iota(0, (int) Settings::SETTINGS.uMaxSkillLevel) );
                }
            }


            inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "uMaxCastSpeedMinLevel_Novice", Settings::SETTINGS.sSpeed.uMaxCastSpeedMinLevel_Novice);
            inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "uMaxCastSpeedMinLevel_Apprentice", Settings::SETTINGS.sSpeed.uMaxCastSpeedMinLevel_Apprentice);
            inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "uMaxCastSpeedMinLevel_Adept", Settings::SETTINGS.sSpeed.uMaxCastSpeedMinLevel_Adept);
            inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "uMaxCastSpeedMinLevel_Expert", Settings::SETTINGS.sSpeed.uMaxCastSpeedMinLevel_Expert);
            inipp::get_value(ini.sections["CastingSpeedFormulaSetting"], "uMaxCastSpeedMinLevel_Master", Settings::SETTINGS.sSpeed.uMaxCastSpeedMinLevel_Master);
            
            
            
            //// FAIL: ==============================================

            inipp::get_value(ini.sections["Fail"], "bCastCanFail_NPC", Settings::SETTINGS.sFailure.bCastCanFail_NPC);
            inipp::get_value(ini.sections["Fail"], "bCastCanFail_Player", Settings::SETTINGS.sFailure.bCastCanFail_Player);

            inipp::get_value(ini.sections["Fail"], "uNoFailMinLevel_Novice", Settings::SETTINGS.sFailure.uNoFailMinLevel_Novice);
            inipp::get_value(ini.sections["Fail"], "uNoFailMinLevel_Apprentice", Settings::SETTINGS.sFailure.uNoFailMinLevel_Apprentice);
            inipp::get_value(ini.sections["Fail"], "uNoFailMinLevel_Adept", Settings::SETTINGS.sFailure.uNoFailMinLevel_Adept);
            inipp::get_value(ini.sections["Fail"], "uNoFailMinLevel_Expert", Settings::SETTINGS.sFailure.uNoFailMinLevel_Expert);
            inipp::get_value(ini.sections["Fail"], "uNoFailMinLevel_Master", Settings::SETTINGS.sFailure.uNoFailMinLevel_Master);
            

            //// FAIL: ==============================================
            inipp::get_value(ini.sections["Backfire"], "bFailCanBackfire_NPC", Settings::SETTINGS.sBackfire.bFailCanBackfire_NPC);
            inipp::get_value(ini.sections["Backfire"], "bFailCanBackfire_Player", Settings::SETTINGS.sBackfire.bFailCanBackfire_Player);

            inipp::get_value(ini.sections["Backfire"], "fBackfireChanceMin", Settings::SETTINGS.sBackfire.fBackfireChanceMin);
            inipp::get_value(ini.sections["Backfire"], "fBackfireChanceMult", Settings::SETTINGS.sBackfire.fBackfireChanceMult);
            inipp::get_value(ini.sections["Backfire"], "fBackfireChanceSLCoeff", Settings::SETTINGS.sBackfire.fBackfireChanceSLCoeff);

            inipp::get_value(ini.sections["Backfire"], "fBackfireDefaultMaxDamage", Settings::SETTINGS.sBackfire.fBackfireDefaultMaxDamage);
            inipp::get_value(ini.sections["Backfire"], "fBackfireDefaultMinDamage", Settings::SETTINGS.sBackfire.fBackfireDefaultMinDamage);

        }
        catch(const std::exception& e)
        {
            const char * errorSt = "!! Failure while parsing some of the args. Detail: "; // 51
            char errorBuffer [351];
            strcat(errorBuffer, errorSt);
            strcat(errorBuffer, e.what());

            return {false, errorSt};
        }
        
        return {true, "All values parsed successfully"};
    }

}

