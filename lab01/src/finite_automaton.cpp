#include <finite_automaton.hpp>

#include <stdexcept>


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
	if (!alphabet_.contains(symbol)) {
		throw std::invalid_argument("[FiniteAutomaton::CheckSymbolIsValid_] There is no such symbol in the alphabet: '" + std::string{symbol} + "'");
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

FiniteAutomaton& FiniteAutomaton::reverse() {
	if (accept_states_.empty()) {
		throw std::invalid_argument("[FiniteAutomaton::reverse] Cannot reverse FA without accept states");
	}

	auto transitions = std::move(transitions_);
	transitions_.clear();

	for (auto&& [from, map_symbol_to_states] : transitions) {
		for (auto&& [symbol, to_states] : map_symbol_to_states) {
			for (auto&& to : to_states) {
				transitions_[to][symbol].insert(from);
			}
		}
	}

	auto tmp = std::move(initial_state_);
	if (accept_states_.size() == 1) {
		initial_state_ = *accept_states_.begin();
		accept_states_.clear();
		accept_states_.insert(std::move(tmp));
	} else {
		initial_state_ = "s0";
		while (states_.contains(initial_state_)) {
			initial_state_ += "0";
		}
		states_.insert(initial_state_);
		for (auto&& state : accept_states_) {
			transitions_[initial_state_]['L'] = {state};
		}
		accept_states_.clear();
		accept_states_.insert(std::move(tmp));
	}

	return *this;
}

std::string FiniteAutomaton::toDotFormat() const {
	std::string dot =
		"digraph DFA{\n"
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
