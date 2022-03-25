#include <grammars/context_free_grammar.hpp>

#include <stdexcept>


ContextFreeGrammar::ContextFreeGrammar(const Grammar& other)
	: Grammar{other}
{
	for (auto&& [from, to] : production_rules_) {
		if (from.size() != 1 || !non_terminal_symbols_.contains(from.front())) {
			throw std::invalid_argument("[ContextFreeGrammar::ContextFreeGrammar] Left part of the production rule can have only one non terminal");
		}
	}
}
