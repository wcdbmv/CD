#include <grammar.hpp>

#include <stdexcept>

#include <utils/set_utils.hpp>


Grammar::Grammar(
	Alphabet terminal_symbols,
	Alphabet non_terminal_symbols,
	ProductionRules production_rules,
	Symbol start_symbol
)
	: terminal_symbols_{std::move(terminal_symbols)}
	, non_terminal_symbols_{std::move(non_terminal_symbols)}
	, production_rules_{std::move(production_rules)}
	, start_symbol_{std::move(start_symbol)}
{
	if (auto common = SetUtils::intersection(terminal_symbols_, non_terminal_symbols_); !common.empty()) {
		throw std::invalid_argument("[Grammar::Grammar] Σ and N contains common symbols: " + SetUtils::toString(common));
	}

	for (auto&& [from, to] : production_rules_) {
		checkProductionRule_(from, to);
	}

	if (!(start_symbol_.empty() && non_terminal_symbols_.empty()
	      || non_terminal_symbols_.contains(start_symbol_))) {
		throw std::invalid_argument("[Grammar::Grammar] S must be in N");
	}
}

void Grammar::checkProductionRule_(const String& from, const String& to) {
	bool was_non_terminal = false;
	for (auto&& symbol : from) {
		if (non_terminal_symbols_.contains(symbol)) {
			was_non_terminal = true;
		} else if (!terminal_symbols_.contains(symbol)) {
			throw std::invalid_argument("[Grammar::checkProductionRule_] Symbols in the left part of the production rule must be in Σ ∪ N");
		}
	}
	if (!was_non_terminal) {
		throw std::invalid_argument("[Grammar::checkProductionRule_] Symbols in the left part of the production rule must contain non-terminal");
	}

	for (auto&& symbol : to) {
		if (!terminal_symbols_.contains(symbol) && !non_terminal_symbols_.contains(symbol)) {
			throw std::invalid_argument("[Grammar::checkProductionRule_] Symbols in the right part of the production rule must be in Σ ∪ N");
		}
	}
}

std::istream& operator>>(std::istream& is, Grammar& grammar) {
	Alphabet terminal_symbols;
	Alphabet non_terminal_symbols;
	ProductionRules production_rules;
	Symbol start_symbol;

	size_t n;
	is >> n;

	for (size_t i = 0; i < n; ++i) {
		Symbol symbol;
		is >> symbol;
		terminal_symbols.insert(std::move(symbol));
	}

	is >> n;

	for (size_t i = 0; i < n; ++i) {
		Symbol symbol;
		is >> symbol;
		non_terminal_symbols.insert(std::move(symbol));
	}

	is >> n;

	for (size_t i = 0; i < n; ++i) {
		String from, to;
		Symbol symbol;
		while ((is >> symbol), symbol != "->") {
			from.push_back(std::move(symbol));
		}

		std::string line;
		std::getline(is, line);
		std::stringstream ss{line};

		while (ss >> symbol) {
			to.push_back(std::move(symbol));
		}

		production_rules.emplace(std::move(from), std::move(to));
	}

	is >> start_symbol;

	grammar = Grammar{
		terminal_symbols,
		non_terminal_symbols,
		production_rules,
		start_symbol
	};

	return is;
}

std::ostream& operator<<(std::ostream& os, const Grammar& grammar) {
	os << grammar.terminal_symbols_.size() << '\n';
	join(grammar.terminal_symbols_, os, " ");
	os << '\n' << grammar.non_terminal_symbols_.size() << '\n';
	join(grammar.non_terminal_symbols_, os, " ");
	os << '\n' << grammar.production_rules_.size() << '\n';
	for (auto&& [from, to] : grammar.production_rules_) {
		join(from, os, " ");
		os << " -> ";
		join(to, os, " ");
		os << '\n';
	}
	os << grammar.start_symbol_ << '\n';
	return os;
}
