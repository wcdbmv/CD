#pragma once

#include <functional>
#include <ostream>
#include <string>
#include <unordered_set>

#include <abstract_syntax_tree.hpp>
#include <hash_combine.hpp>


struct State {
	std::string id;
	std::set<size_t> indices;

	bool operator==(const State&) const = default;
};

template<>
struct std::hash<State> {
	std::size_t operator()(const State& state) const noexcept {
		auto h1 = std::hash<std::string>{}(state.id);
		auto h2 = hash_container(state.indices);
		hash_combine(h1, h2);
		return h1;
	}
};

std::ostream& operator<<(std::ostream& os, const State& state);


class DeterministicFiniteAutomaton {
public:
	explicit DeterministicFiniteAutomaton(std::string_view expression);

	void AddState(const State& state);
	void AddTransition(const State& from, const State& to, char symbol);
	void SetInitialState(const State& state);
	void AddAcceptState(const State& state);

	std::string convertAstToDotFormat() const;
	std::string convertDfaToDotFormat() const;

private:
	AbstractSyntaxTreeNode* root_;
	AbstractSyntaxTree ast_;

	// Конечное множество состояний Q
	std::unordered_set<State> states_;

	// Конечное множество входных символов, называемое алфавитом Σ
	std::unordered_set<char> alphabet_;

	// Функция переходов δ: Q × Σ → Q
	std::unordered_map<State, std::unordered_map<char, State>> transitions_;

	// Начальное состояние q0 ∈ Q
	State initial_state_;

	// Множество конечных состояний F ⊆ Q
	std::unordered_set<State> accept_states_;

	void CheckStateIsValid_(const State& state);
	void CheckTransitionIsValid_(const State& from, char symbol);
};
