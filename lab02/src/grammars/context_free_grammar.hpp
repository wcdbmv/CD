#pragma once

#include <grammars/grammar.hpp>


class ContextFreeGrammar : public Grammar {
public:
	explicit ContextFreeGrammar(const Grammar& other);
};
