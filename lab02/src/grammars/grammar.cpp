#include <grammars/grammar.hpp>

#include <fstream>
#include <stdexcept>

#include <utils/set_utils.hpp>


Grammar::Grammar(
	Alphabet non_terminal_symbols,
	Alphabet terminal_symbols,
	ProductionRules production_rules,
	Symbol start_symbol
)
	: non_terminal_symbols_{std::move(non_terminal_symbols)}
	, terminal_symbols_{std::move(terminal_symbols)}
	, production_rules_{std::move(production_rules)}
	, start_symbol_{std::move(start_symbol)}
{
	if (auto common = SetUtils::intersection(terminal_symbols_, non_terminal_symbols_); !common.empty()) {
		throw std::invalid_argument("[Grammar::Grammar] Σ and N contains common symbols: " + SetUtils::toString(common));
	}

	for (auto&& [from, tos] : production_rules_) {
		for (auto&& to : tos) {
			checkProductionRule_(from, to);
		}
	}

	if (!(   start_symbol_.empty() && non_terminal_symbols_.empty()
	      || non_terminal_symbols_.contains(start_symbol_))) {
		throw std::invalid_argument("[Grammar::Grammar] S must be in N");
	}
}

void Grammar::checkProductionRule_(const String& from, const String& to) const {
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

size_t Grammar::productionRulesCount() const {
	size_t n = 0;
	for (auto&& [from, tos] : production_rules_) {
		n += tos.size();
	}
	return n;
}

std::istream& operator>>(std::istream& is, Grammar& grammar) {
	Alphabet non_terminal_symbols;
	Alphabet terminal_symbols;
	ProductionRules production_rules;
	Symbol start_symbol;

	size_t n;
	is >> n;

	for (size_t i = 0; i < n; ++i) {
		Symbol symbol;
		is >> symbol;
		non_terminal_symbols.insert(std::move(symbol));
	}

	is >> n;

	for (size_t i = 0; i < n; ++i) {
		Symbol symbol;
		is >> symbol;
		terminal_symbols.insert(std::move(symbol));
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

		production_rules[from].insert(std::move(to));
	}

	is >> start_symbol;

	grammar = Grammar{
		non_terminal_symbols,
		terminal_symbols,
		production_rules,
		start_symbol
	};

	return is;
}

std::ostream& operator<<(std::ostream& os, const Grammar& grammar) {
	os << grammar.non_terminal_symbols_.size() << '\n';
	join(grammar.non_terminal_symbols_, os, " ");
	os << '\n' << grammar.terminal_symbols_.size() << '\n';
	join(grammar.terminal_symbols_, os, " ");
	os << '\n' << grammar.productionRulesCount() << '\n';
	for (auto&& [from, tos] : grammar.production_rules_) {
		for (auto&& to : tos) {
			join(from, os, " ");
			os << " -> ";
			join(to, os, " ");
			os << '\n';
		}
	}
	os << grammar.start_symbol_ << '\n';
	return os;
}

Grammar Grammar::readFromFile(const std::filesystem::path& path) {
	Grammar grammar;
	std::ifstream stream{path};
	stream >> grammar;
	return grammar;
}

void Grammar::writeToFile(const std::filesystem::path& path) const {
	std::ofstream stream{path};
	stream << *this;
}
