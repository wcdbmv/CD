#pragma once

#include <istream>
#include <ostream>

#include <types/common.hpp>


class Grammar {
public:
	explicit Grammar(
		Alphabet terminal_symbols = {},
		Alphabet non_terminal_symbols = {},
		ProductionRules production_rules = {},
		Symbol start_symbol = {}
	);

	friend std::istream& operator>>(std::istream& is, Grammar& grammar);
	friend std::ostream& operator<<(std::ostream& os, const Grammar& grammar);

private:
	// Σ — набор (алфавит) терминальных символов
	Alphabet terminal_symbols_;

	// N — набор (алфавит) нетерминальных символов
	Alphabet non_terminal_symbols_;

	// P — набор правил вида: «левая часть» → «правая часть», где
	//   — «левая часть» — непустая последоавтельность терминалов и нетерминалов,
	//     содержащая хотя бы один нетерминал
	//   — «правая часть» — любая последоавтельность терминалов и нетерминалов
	ProductionRules production_rules_;

	// S — стартовый (или начальный) символ грамматики из набора нетерминалов
	Symbol start_symbol_;

	void checkProductionRule_(const String& from, const String& to);
};