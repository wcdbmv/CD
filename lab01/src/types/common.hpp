#pragma once

#include <set>
#include <string>
#include <unordered_map>


template <typename T>
using Set = std::set<T>;

template <typename T, typename U>
using UMap = std::unordered_map<T, U>;

using State = std::string;
using Symbol = char;

using States = Set<State>;
using Alphabet = Set<Symbol>;

using Transitions = UMap<State, UMap<Symbol, States>>;
