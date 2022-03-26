#pragma once

#include <optional>

#include <grammars/grammar.hpp>


class ContextFreeGrammar : public Grammar {
public:
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

public:
	String calcBestLinearOrder_() const;
};
