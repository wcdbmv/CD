#include <deterministic_finite_automaton.hpp>

#include <experimental/iterator>
#include <sstream>

#include <recursive_descent_parser.hpp>
#include <set_union.hpp>


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

std::unordered_set<char> sCalculateAlphabet(std::string_view expression) {
	std::unordered_set<char> alphabet;

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

}  // namespace


DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(std::string_view expression)
	: root_{RecursiveDescentParser{}.parse("(" + std::string(expression) + ")#")}
	, ast_{root_}
	, alphabet_{sCalculateAlphabet(expression)}
{
	auto& leaf_to_index = ast_.leafToIndex();
	auto& index_to_leaf = ast_.indexToLeaf();
	auto& first_pos = ast_.firstPos();
	auto& follow_pos = ast_.followPos();

	auto* accept_node = sFindAcceptNode(root_);
	auto accept_node_index = leaf_to_index[accept_node];

	std::set<std::set<size_t>> non_visited;

	initial_state_ = sToString(first_pos[root_]);
	states_.insert(initial_state_);
	non_visited.insert(first_pos[root_]);
	if (first_pos[root_].contains(accept_node_index)) {
		accept_states_.insert(initial_state_);
	}

	while (!non_visited.empty()) {
		auto s = *non_visited.begin();
		non_visited.erase(non_visited.begin());
		for (auto a : alphabet_) {
			std::set<size_t> u;
			for (auto p : s) {
				auto* p_leaf = index_to_leaf[p];
				if (p_leaf->data == a) {
					set_append(u, follow_pos[p_leaf]);
				}
			}
			auto u_str = sToString(u);
			if (!states_.contains(u_str)) {
				states_.insert(u_str);
				if (u.contains(accept_node_index)) {
					accept_states_.insert(u_str);
				}
				non_visited.insert(u);
			}
			transitions_[sToString(s)][a] = u_str;
		}
	}
}

void DeterministicFiniteAutomaton::AddState(const State& state) {
	states_.insert(state);
}

void DeterministicFiniteAutomaton::CheckStateIsValid_(const State& state) {
	if (!states_.contains(state)) {
		throw std::invalid_argument("[DeterministicFiniteAutomaton::CheckStateIsValid_] There is no such registered state: " + sToString(state));
	}
}

void DeterministicFiniteAutomaton::AddTransition(const State& from, const State& to, char symbol) {
	CheckStateIsValid_(from);
	CheckStateIsValid_(to);
	transitions_[from][symbol] = to;
}

void DeterministicFiniteAutomaton::SetInitialState(const State& state) {
	CheckStateIsValid_(state);
	initial_state_ = state;
}

void DeterministicFiniteAutomaton::AddAcceptState(const State& state) {
	CheckStateIsValid_(state);
	accept_states_.insert(state);
}

void DeterministicFiniteAutomaton::CheckTransitionIsValid_(const State& from, char symbol) {
	if (auto it1 = transitions_.find(from); it1 != transitions_.end()) {
		auto& from_state_transitions = it1->second;
		if (auto it2 = from_state_transitions.find(symbol); it2 != from_state_transitions.end()) {
			return;
		}
	}

	throw std::invalid_argument("[DeterministicFiniteAutomaton::CheckTransitionIsValid_] There is no such registered transition: from " + sToString(from) + " by '" + symbol + "'");
}

std::string DeterministicFiniteAutomaton::convertAstToDotFormat() const {
	return ast_.convertToDotFormat();
}

std::string DeterministicFiniteAutomaton::convertDfaToDotFormat() const {
	std::string s;

	for (auto&& state : states_) {
		s += "\t";
		if (initial_state_ == state) {
			s += "\"\" -> ";
		}
		s += "\"" + state + "\"";
		if (accept_states_.contains(state)) {
			s += " [peripheries=2]";
		}
		s += "\n";
	}
	s += "\n";

	for (auto&& [from, map_symbol_to_state] : transitions_) {
		for (auto&& [symbol, to] : map_symbol_to_state) {
			s += "\t\"" + from + "\" -> \"" + to + "\" [label=\"" + symbol + "\"]\n";
		}
	}

	return "digraph DFA {\n"
	       "\trankdir=\"LR\"\n"
	       "\t\"\" [shape=none]\n\n" +
	       s +
	       "}\n";
}
