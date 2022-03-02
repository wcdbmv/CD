#include <deterministic_finite_automaton.hpp>

#include <experimental/iterator>
#include <sstream>
#include <stack>
#include <queue>

#include <recursive_descent_parser.hpp>
#include <set_utils.hpp>


namespace {

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set) {
	os << "{";
	std::copy(set.begin(), set.end(), std::experimental::make_ostream_joiner(os, ", "));
	os << "}";
	return os;
}

std::string sToString(const auto& data) {
	std::stringstream ss;
	ss << data;
	return ss.str();
}


FiniteAutomaton::States sMove(const FiniteAutomaton& fa, const FiniteAutomaton::States& T, FiniteAutomaton::Symbol a) {
	FiniteAutomaton::States states;
	for (auto&& s : T) {
		setAppend(states, fa.transition(s, a));
	}
	return states;
}

FiniteAutomaton::States sLambdaClosure(const FiniteAutomaton& fa, const FiniteAutomaton::States& T) {
	auto closure = T;
	auto stack = T;

	while (!stack.empty()) {
		auto t = *stack.begin();
		stack.erase(stack.begin());

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
	auto initial_state_str = sToString(initial_state);
	FiniteAutomaton::States states = {initial_state_str};

	FiniteAutomaton::States accept_states;
	if (!setIntersection(initial_state, fa.accept_states()).empty()) {
		accept_states.insert(initial_state_str);
	}

	FiniteAutomaton::Transitions transitions;

	std::set<FiniteAutomaton::States> non_visited;
	non_visited.insert(initial_state);

	while (!non_visited.empty()) {
		auto T = *non_visited.begin();
		non_visited.erase(non_visited.begin());

		for (auto a : fa.alphabet()) {
			auto U = sLambdaClosure(fa, sMove(fa, T, a));
			auto U_str = sToString(U);
			if (!states.contains(U_str)) {
				states.insert(U_str);
				if (!setIntersection(U, fa.accept_states()).empty()) {
					accept_states.insert(U_str);
				}
				non_visited.insert(U);
			}
			transitions[sToString(T)][a] = {U_str};
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

FiniteAutomaton::Alphabet sCalculateAlphabet(std::string_view expression) {
	FiniteAutomaton::Alphabet alphabet;

	for (auto c : expression) {
		if (c != '(' && c != ')' && c != '|' && c != '*') {
			alphabet.insert(c);
		}
	}

	return alphabet;
}

AbstractSyntaxTreeNode* sFindAcceptNode(AbstractSyntaxTreeNode* node) {
	if (node->isLeaf()) {
		if (node->data == '#') {
			return node;
		}
		return nullptr;
	}

	for (auto child : {node->left, node->right}) {
		if (child) {
			auto accept_node = sFindAcceptNode(child);
			if (accept_node) {
				return accept_node;
			}
		}
	}

	return nullptr;
}

FiniteAutomaton sBuildDfaFromRegex(std::string_view expression) {
	auto alphabet = sCalculateAlphabet(expression);

	auto root = RecursiveDescentParser{}.parse("(" + std::string{expression} + ")#");
	auto ast = AbstractSyntaxTree{root};

	auto& leaf_to_index = ast.leafToIndex();
	auto& index_to_leaf = ast.indexToLeaf();
	auto& first_pos = ast.firstPos();
	auto& follow_pos = ast.followPos();

	auto accept_node = sFindAcceptNode(root);
	auto accept_node_index = leaf_to_index[accept_node];

	auto initial_state = sToString(first_pos[root]);
	auto states = FiniteAutomaton::States{initial_state};

	FiniteAutomaton::States accept_states;
	if (first_pos[root].contains(accept_node_index)) {
		accept_states = states;
	}

	FiniteAutomaton::Transitions transitions;

	std::set<std::set<size_t>> non_visited;
	non_visited.insert(first_pos[root]);

	while (!non_visited.empty()) {
		auto s = *non_visited.begin();
		non_visited.erase(non_visited.begin());

		for (auto a : alphabet) {
			std::set<size_t> u;
			for (auto p : s) {
				auto* p_leaf = index_to_leaf[p];
				if (p_leaf->data == a) {
					setAppend(u, follow_pos[p_leaf]);
				}
			}
			auto u_str = sToString(u);
			if (!states.contains(u_str)) {
				states.insert(u_str);
				if (u.contains(accept_node_index)) {
					accept_states.insert(u_str);
				}
				non_visited.insert(u);
			}
			transitions[sToString(s)][a] = {u_str};
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
