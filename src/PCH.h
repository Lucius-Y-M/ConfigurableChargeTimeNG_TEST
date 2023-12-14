#pragma once

#include "../include/RE/Skyrim.h"
#include "../include/SKSE/SKSE.h"



#include <unordered_set>
#include <vector>




// ==== Alias

using u32 = uint32_t;
using u8 = uint8_t;

template<typename T>
using Some = std::optional<T>;
using None = std::nullopt_t;



template<typename T>
using HashSet = std::unordered_set<T>;
template<typename T>
using Set = std::set<T>;
template<typename T>
using Vec = std::vector<T>;
template<typename T, typename U>
using HashMap = std::unordered_map<T, U>;
template<typename T, typename U, typename H>
using HashMapHash = std::unordered_map<T, U, H>;





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

using ST = RE::MagicSystem::SpellType;
using AV = RE::ActorValue;

template <typename T, typename U>
using tup = std::tuple<T, U>;

namespace logger = SKSE::log;
using namespace std::literals;





#include "inclu/settings.h"
#include "inclu/consts.h"
#include "inclu/hook.h"
#include "inclu/failcheck.h"
#include "inclu/distribute.h"
#include "inclu/ini.h"