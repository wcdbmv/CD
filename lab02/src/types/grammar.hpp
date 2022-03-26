#pragma once

#include <string>

#include <types/common.hpp>


using Symbol = std::string;
using Alphabet = Set<Symbol>;

using String = Vector<Symbol>;

using ProductionRules = UMap<String, Set<String>>;

const String kEpsilon{};
