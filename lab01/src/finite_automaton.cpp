#include <finite_automaton.hpp>

#include <stdexcept>

#include <set_utils.hpp>
#include <utility>


namespace {

void sForAllTransitions(const FiniteAutomaton::Transitions& transitions, auto onTransition) {
	for (auto&& [from, map_symbol_to_states] : transitions) {
		for (auto&& [symbol, to_states] : map_symbol_to_states) {
			for (auto&& to : to_states) {
				onTransition(from, symbol, to);
			}
		}
	}
}

}  // namespace


FiniteAutomaton::FiniteAutomaton(
	States states /* = {} */,
	Alphabet alphabet /* = {} */,
	Transitions transitions /* = {} */,
	State initial_state /* = {} */,
	States accept_states /* = {} */
)
	: states_{std::move(states)}
	, alphabet_{std::move(alphabet)}
	, transitions_{std::move(transitions)}
	, initial_state_{std::move(initial_state)}
	, accept_states_{std::move(accept_states)}
{
	checkStateIsValid_(initial_state_);

	for (auto&& accept_state : accept_states_) {
		checkStateIsValid_(accept_state);
	}

	for (auto&& [from, map_symbol_to_states] : transitions_) {
		checkStateIsValid_(from);
		for (auto&& [symbol, to_states] : map_symbol_to_states) {
			checkSymbolIsValid_(symbol);
			for (auto&& to : to_states) {
				checkStateIsValid_(to);
			}
		}
	}
}

void FiniteAutomaton::checkStateIsValid_(const State& state) const {
	if (!states_.contains(state)) {
		throw std::invalid_argument("[FiniteAutomaton::CheckStateIsValid_] There is no such registered state: \"" + state + "\"");
	}
}

void FiniteAutomaton::checkSymbolIsValid_(Symbol symbol) const {
	if (symbol != 'L' && !alphabet_.contains(symbol)) {
		throw std::invalid_argument("[FiniteAutomaton::CheckSymbolIsValid_] There is no such symbol in the alphabet: '" + std::string{symbol} + "'");
	}
}

void FiniteAutomaton::forAllTransitions_(OnTransition onTransition) {
	sForAllTransitions(transitions_, std::move(onTransition));
}

void FiniteAutomaton::createInitialState_(const States& initial_states) {
	initial_state_ = "s0";
	while (states_.contains(initial_state_)) {
		initial_state_ += "0";
	}

	states_.insert(initial_state_);
	for (auto&& state : initial_states) {
		transitions_[initial_state_]['L'].insert(state);
	}
}

auto FiniteAutomaton::states() const -> const States& {
	return states_;
}

auto FiniteAutomaton::alphabet() const -> const Alphabet& {
	return alphabet_;
}

auto FiniteAutomaton::transitions() const -> const Transitions& {
	return transitions_;
}

auto FiniteAutomaton::initial_state() const -> const State& {
	return initial_state_;
}

auto FiniteAutomaton::accept_states() const -> const States& {
	return accept_states_;
}

auto FiniteAutomaton::transition(const State& from, Symbol symbol) const -> States {
	if (auto it_from = transitions_.find(from); it_from != transitions_.end()) {
		if (auto it = it_from->second.find(symbol); it != it_from->second.end()) {
			return it->second;
		}
	}
	return {};
}

void FiniteAutomaton::reverse() {
	if (accept_states_.empty()) {
		throw std::invalid_argument("[FiniteAutomaton::reverse] Cannot reverse FA without accept states");
	}

	auto transitions = std::move(transitions_);
	transitions_.clear();

	sForAllTransitions(transitions, [&](const State& from, Symbol symbol, const State& to) {
		transitions_[to][symbol].insert(from);
	});

	auto tmp = std::move(initial_state_);
	if (accept_states_.size() == 1) {
		initial_state_ = popFirst(accept_states_);
		accept_states_.insert(std::move(tmp));
	} else {
		createInitialState_(accept_states_);
		accept_states_.clear();
		accept_states_.insert(std::move(tmp));
	}
}

FiniteAutomaton FiniteAutomaton::reversed() {
	auto fa = *this;
	fa.reverse();
	return fa;
}

void FiniteAutomaton::rename() {
	std::unordered_map<std::string, std::string> translation;

	FiniteAutomaton::States new_states;

	size_t i = 0;
	for (auto&& state : states_) {
		auto new_state = std::to_string(i);
		translation[state] = new_state;
		new_states.insert(std::move(new_state));
		++i;
	}

	FiniteAutomaton::Transitions new_transitions;
	forAllTransitions_([&](const State& from, Symbol symbol, const State& to) {
		new_transitions[translation[from]][symbol].insert(translation[to]);
	});

	auto new_initial_state = translation[initial_state_];

	FiniteAutomaton::States new_accept_states;
	for (auto&& accept_state : accept_states_) {
		new_accept_states.insert(translation[accept_state]);
	}

	states_ = std::move(new_states);
	transitions_ = std::move(new_transitions);
	initial_state_ = std::move(new_initial_state);
	accept_states_ = std::move(new_accept_states);
}

void FiniteAutomaton::deleteUnreachableStates() {
	auto fa = *this;
	fa.reverse();

	auto alphabet = fa.alphabet_;
	alphabet.insert('L');

	States visited;
	States need_to_visit = {fa.initial_state_};

	while (!need_to_visit.empty()) {
		auto state = popFirst(need_to_visit);
		visited.insert(state);

		for (auto symbol : alphabet) {
			setAppend(need_to_visit, setDifference(fa.transition(state, symbol), visited));
		}
	}

	for (auto&& unreachable : setDifference(fa.states_, visited)) {
		deleteState(unreachable);
	}
}

void FiniteAutomaton::deleteState(const State& state) {
	auto it = states_.find(state);
	if (it == states_.end()) {
		return;
	}

	states_.erase(it);

	if (initial_state_ == state) {
		FiniteAutomaton::States initial_states;
		for (auto&& [symbol, to_states] : transitions_[state]) {
			setAppend(initial_states, to_states);
		}
		if (!initial_states.empty()) {
			createInitialState_(initial_states);
		}
	}

	transitions_.erase(state);

	for (auto&& [from, map_symbol_to_states] : transitions_) {
		for (auto&& [symbol, to_states] : map_symbol_to_states) {
			if (auto it = to_states.find(state); it != to_states.end()) {
				to_states.erase(it);
			}
		}
	}

	accept_states_.erase(state);
}

std::string FiniteAutomaton::toDotFormat() const {
	std::string dot =
		"digraph DFA {\n"
		"\trankdir=\"LR\"\n"
		"\t\"\" [shape=none]\n\n";

	for (auto&& state : states_) {
		dot += "\t";
		if (initial_state_ == state) {
			dot += "\"\" -> ";
		}
		dot += "\"" + state + "\"";
		if (accept_states_.contains(state)) {
			dot += " [peripheries=2]";
		}
		dot += "\n";
	}
	dot += "\n";

	for (auto&& [from, map_symbol_to_state] : transitions_) {
		const auto from_str = "\t\"" + from + "\" -> \"";
		for (auto&& [symbol, to_states] : map_symbol_to_state) {
			const auto symbol_str = "\" [label=\"" + std::string{symbol} + "\"]\n";
			for (auto&& to : to_states) {
				dot += from_str;
				dot += to;
				dot += symbol_str;
			}
		}
	}
	dot += "}\n";

	return dot;
}