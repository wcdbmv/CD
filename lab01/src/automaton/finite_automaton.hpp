#pragma once

#include <functional>
#include <set>
#include <string>
#include <unordered_map>

#include <types/common.hpp>


class FiniteAutomaton {
public:
	explicit FiniteAutomaton(
		States states = {},
		Alphabet alphabet = {},
		Transitions transitions = {},
		State initial_state = {},
		States accept_states = {}
	);

	const States& states() const { return states_; }
	const Alphabet& alphabet() const { return alphabet_; }
	const Transitions& transitions() const { return transitions_; }
	const State& initial_state() const { return initial_state_; }
	const States& accept_states() const { return accept_states_; }

	States transition(const State& from, Symbol symbol) const;

	void reverse();  // Do not use this from DFA
	FiniteAutomaton reversed() const;

	void rename();
	void deleteUnreachableStates();
	void deleteState(const State& state);

	std::string toDotFormat() const;

protected:
	// Конечное множество состояний Q
	States states_;

	// Конечное множество входных символов, называемое алфавитом Σ
	Alphabet alphabet_;

	// Функция переходов δ: Q × Σ → Q^n
	Transitions transitions_;

	// Начальное состояние q0 ∈ Q
	State initial_state_;

	// Множество конечных состояний F ⊆ Q
	States accept_states_;

	void checkStateIsValid_(const State& state) const;
	void checkSymbolIsValid_(Symbol symbol) const;

	using OnTransition = std::function<void(const State& from, Symbol symbol, const State& to)>;
	void forAllTransitions_(OnTransition onTransition);

	void createInitialState_(const States& initial_states);
};
