#pragma once

#include <abstract_syntax_tree.hpp>
#include <finite_automaton.hpp>


class DeterministicFiniteAutomaton : public FiniteAutomaton {
public:
	explicit DeterministicFiniteAutomaton(const FiniteAutomaton& other);
	explicit DeterministicFiniteAutomaton(std::string_view expression);
};
