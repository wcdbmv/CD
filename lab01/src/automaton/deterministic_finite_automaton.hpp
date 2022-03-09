#pragma once

#include <automaton/finite_automaton.hpp>


class DeterministicFiniteAutomaton : public FiniteAutomaton {
public:
	explicit DeterministicFiniteAutomaton(const FiniteAutomaton& other);
	explicit DeterministicFiniteAutomaton(std::string_view expression);

	bool accept(std::string_view s);
};
