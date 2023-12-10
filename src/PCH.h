#pragma once

#include "../include/RE/Skyrim.h"
#include "../include/SKSE/SKSE.h"

#include "inclu/settings.h"
#include "inclu/consts.h"
#include "inclu/hook.h"
#include "inclu/failcheck.h"
#include "inclu/distribute.h"


#include <unordered_set>

// ==== Alias


template<typename T>
using Vec = std::vector<T>;
template<class T>
using Set = std::set<T>;


using u32 = uint32_t;

using Keyword = RE::BGSKeyword;
using Sound = RE::TESSound;
using Spell = RE::SpellItem;
using ObjectRef = RE::TESObjectREFR;
using FormType = RE::FormType;
using Actor = RE::Actor;


template <typename T>
using BSTES = RE::BSTEventSource<T>;
using BSNEC = RE::BSEventNotifyControl;
using SKSEMI = SKSE::MessagingInterface;
using FID = RE::FormID;

namespace logger = SKSE::log;
using namespace std::literals;