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


std::ostream& operator<<(std::ostream& os, const State& state) {
	os << "State(id=\"" << state.id << "\", indices=";
	std::copy(state.indices.begin(), state.indices.end(), std::experimental::make_ostream_joiner(os, " "));
	os << "})";
	return os;
}


DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(std::string_view expression)
	: root_{RecursiveDescentParser{}.parse("(" + std::string(expression) + ")#")}
	, ast_{root_}
	, alphabet_{sCalculateAlphabet(expression)}
{
	auto& leaf_to_index = ast_.leafToIndex();
	auto& index_to_leaf = ast_.indexToLeaf();
	auto& first_pos = ast_.firstPos();
	auto& follow_pos = ast_.followPos();

	std::unordered_set<State> non_visited;

	initial_state_ = State{sToString(first_pos[root_]), first_pos[root_]};
	states_.insert(initial_state_);
	non_visited.insert(initial_state_);

	while (!non_visited.empty()) {
		State s = *non_visited.begin();
		non_visited.erase(non_visited.begin());
		for (auto a : alphabet_) {
			State u;
			for (auto p : s.indices) {
				auto* p_leaf = index_to_leaf[p];
				if (p_leaf->data == a) {
					set_append(u.indices, follow_pos[p_leaf]);
				}
			}
			u.id = sToString(u.indices);
			if (!states_.contains(u)) {
				states_.insert(u);
				non_visited.insert(u);
			}
			transitions_[s][a] = u;
		}
	}

	auto* accept_node = sFindAcceptNode(root_);
	auto accept_node_index = leaf_to_index[accept_node];

	for (auto&& state : states_) {
		if (state.indices.contains(accept_node_index)) {
			accept_states_.insert(state);
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
		s += "\"" + state.id + "\"";
		if (accept_states_.contains(state)) {
			s += " [peripheries=2]";
		}
		s += "\n";
	}
	s += "\n";

	for (auto&& [from, map_symbol_to_state] : transitions_) {
		for (auto&& [symbol, to] : map_symbol_to_state) {
			s += "\t\"" + from.id + "\" -> \"" + to.id + "\" [label=\"" + symbol + "\"]\n";
		}
	}

	return "digraph DFA {\n"
	       "\trankdir=\"LR\"\n"
	       "\t\"\" [shape=none]\n\n" +
	       s +
	       "}\n";
}
