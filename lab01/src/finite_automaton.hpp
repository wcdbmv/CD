#pragma once

#include <functional>
#include <set>
#include <string>
#include <unordered_map>


class FiniteAutomaton {
public:
	template <typename T>
	using Set = std::set<T>;

	template <typename T, typename U>
	using Map = std::unordered_map<T, U>;

	using State = std::string;
	using Symbol = char;

	using States = Set<State>;
	using Alphabet = Set<Symbol>;
	using Transitions = Map<State, Map<Symbol, States>>;

public:
	explicit FiniteAutomaton(
		States states = {},
		Alphabet alphabet = {},
		Transitions transitions = {},
		State initial_state = {},
		States accept_states = {}
	);

	const States& states() const;
	const Alphabet& alphabet() const;
	const Transitions& transitions() const;
	const State& initial_state() const;
	const States& accept_states() const;

	States transition(const State& from, Symbol symbol) const;

	void reverse();  // Do not use this from DFA
	FiniteAutomaton reversed();

	void rename();
	void deleteUnreachableStates();
	void deleteState(const State& state);

	std::string toDotFormat() const;

protected:
	// Конечное множество состояний Q
	States states_;

	// Конечное множество входных символов, называемое алфавитом Σ
	Alphabet alphabet_;

	// Функция переходов δ: Q × Σ → Q
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
