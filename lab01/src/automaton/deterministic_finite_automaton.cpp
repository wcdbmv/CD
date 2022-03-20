#include <automaton/deterministic_finite_automaton.hpp>

#include <parser/abstract_syntax_tree.hpp>
#include <parser/recursive_descent_parser.hpp>
#include <utils/set_utils.hpp>


namespace {

States sMove(const FiniteAutomaton& fa, const States& T, Symbol a) {
	States states;
	for (auto&& s : T) {
		SetUtils::append(states, fa.transition(s, a));
	}
	return states;
}

States sLambdaClosure(const FiniteAutomaton& fa, const States& T) {
	auto closure = T;
	auto stack = T;

	while (!stack.empty()) {
		auto t = SetUtils::popFirst(stack);

		for (auto&& u : fa.transition(t, 'L')) {
			if (!closure.contains(u)) {
				closure.insert(u);
				stack.insert(u);
			}
		}
	}

	return closure;
}

FiniteAutomaton sBuildDfaFromFa(const FiniteAutomaton& fa) {
	auto initial_state = sLambdaClosure(fa, {fa.initial_state()});
	auto initial_state_str = SetUtils::toString(initial_state);
	States states = {initial_state_str};

	States accept_states;
	if (!SetUtils::intersection(initial_state, fa.accept_states()).empty()) {
		accept_states.insert(initial_state_str);
	}

	Transitions transitions;

	std::set<States> non_visited;
	non_visited.insert(initial_state);

	while (!non_visited.empty()) {
		auto T = *non_visited.begin();
		non_visited.erase(non_visited.begin());

		for (auto a : fa.alphabet()) {
			auto U = sLambdaClosure(fa, sMove(fa, T, a));
			auto U_str = SetUtils::toString(U);
			if (!states.contains(U_str)) {
				states.insert(U_str);
				if (!SetUtils::intersection(U, fa.accept_states()).empty()) {
					accept_states.insert(U_str);
				}
				non_visited.insert(U);
			}
			transitions[SetUtils::toString(T)][a] = {U_str};
		}
	}

	auto result = FiniteAutomaton{
		std::move(states),
		fa.alphabet(),
		std::move(transitions),
		std::move(initial_state_str),
		std::move(accept_states)
	};

	//result.deleteState("{}");
	result.deleteUnreachableStates();

	return result;
}

Alphabet sCalculateAlphabet(std::string_view expression) {
	Alphabet alphabet;

	for (auto c : expression) {
		if (c != '(' && c != ')' && c != '|' && c != '*') {
			alphabet.insert(c);
		}
	}

	return alphabet;
}

FiniteAutomaton sBuildDfaFromRegex(std::string_view expression) {
	auto alphabet = sCalculateAlphabet(expression);

	auto root = RecursiveDescentParser{}.parse("(" + std::string{expression} + ")#");
	auto ast = AbstractSyntaxTree{root};

	auto& leaf_to_index = ast.leafToIndex();
	auto& index_to_leaf = ast.indexToLeaf();
	auto& first_pos = ast.firstPos();
	auto& follow_pos = ast.followPos();

	auto* accept_node = ast.findLeaf('#');
	auto accept_node_index = leaf_to_index[accept_node];

	auto initial_state = SetUtils::toString(first_pos[root]);
	auto states = States{initial_state};

	States accept_states;
	if (first_pos[root].contains(accept_node_index)) {
		accept_states = states;
	}

	Transitions transitions;

	std::set<std::set<size_t>> non_visited;
	non_visited.insert(first_pos[root]);

	while (!non_visited.empty()) {
		auto s = SetUtils::popFirst(non_visited);

		for (auto a : alphabet) {
			std::set<size_t> u;
			for (auto p : s) {
				auto* p_leaf = index_to_leaf[p];
				if (p_leaf->data == a) {
					SetUtils::append(u, follow_pos[p_leaf]);
				}
			}
			auto u_str = SetUtils::toString(u);
			if (!states.contains(u_str)) {
				states.insert(u_str);
				if (u.contains(accept_node_index)) {
					accept_states.insert(u_str);
				}
				non_visited.insert(u);
			}
			transitions[SetUtils::toString(s)][a] = {u_str};
		}
	}

	return FiniteAutomaton{
		std::move(states),
		std::move(alphabet),
		std::move(transitions),
		std::move(initial_state),
		std::move(accept_states),
	};
}

}  // namespace


DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(const FiniteAutomaton& other)
	: FiniteAutomaton{sBuildDfaFromFa(other)}
{
}

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(std::string_view expression)
	: FiniteAutomaton{sBuildDfaFromRegex(expression)}
{
}

bool DeterministicFiniteAutomaton::accept(std::string_view s) {
	auto state = initial_state_;
	for (auto c : s) {
		auto next = transition(state, c);
		if (next.empty()) {
			return false;
		}
		state = *next.begin();
	}
	return accept_states_.contains(state);
}
