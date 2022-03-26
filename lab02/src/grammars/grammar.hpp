#pragma once

#include <filesystem>
#include <istream>
#include <ostream>

#include <types/common.hpp>


class Grammar {
public:
	explicit Grammar(
		Alphabet non_terminal_symbols = {},
		Alphabet terminal_symbols = {},
		ProductionRules production_rules = {},
		Symbol start_symbol = {}
	);

	friend std::istream& operator>>(std::istream& is, Grammar& grammar);
	friend std::ostream& operator<<(std::ostream& os, const Grammar& grammar);

	bool operator==(const Grammar& other) const = default;

	static Grammar readFromFile(const std::filesystem::path& path);
	void writeToFile(const std::filesystem::path& path) const;

	size_t productionRulesCount() const;

protected:
	// N — набор (алфавит) нетерминальных символов
	Alphabet non_terminal_symbols_;

	// Σ — набор (алфавит) терминальных символов
	Alphabet terminal_symbols_;

	// P — набор правил вида: «левая часть» → «правая часть», где
	//   — «левая часть» — непустая последоавтельность терминалов и нетерминалов,
	//     содержащая хотя бы один нетерминал
	//   — «правая часть» — любая последоавтельность терминалов и нетерминалов
	ProductionRules production_rules_;

	// S — стартовый (или начальный) символ грамматики из набора нетерминалов
	Symbol start_symbol_;

	void checkProductionRule_(const String& from, const String& to);
};
