#pragma once

#include <optional>

#include <grammars/grammar.hpp>


class ContextFreeGrammar : public Grammar {
public:
	explicit ContextFreeGrammar(
		Alphabet non_terminal_symbols = {},
		Alphabet terminal_symbols = {},
		ProductionRules production_rules = {},
		Symbol start_symbol = {}
	);

	explicit ContextFreeGrammar(const Grammar& other);
	explicit ContextFreeGrammar(Grammar&& other);

	void eliminateLeftRecursion();

	void greibachNormalForm(std::optional<String> mb_order = std::nullopt);

protected:
	void checkContextFree_();

	bool hasImmediateLeftRecursionProductionRules_(const Symbol& A);
	void eliminateImmediateLeftRecursion_(
		const Symbol& A,
		Alphabet& new_non_terminal_symbols,
		ProductionRules& new_production_rules
	);

	String calcBestLinearOrder_() const;
};
