#include <iostream>

#include <grammars/context_free_grammar.hpp>


int main() {
	/*Grammar grammar{
		{"A", "B", "C"},
		{"a", "b", "c"},
		{
			{{"A"}, {"A", "a"}},
			{{"B"}, {"b", "B"}},
			{{"C", "a"}, {"a"}},
		},
		"A",
	};*/

	auto grammar = Grammar::readFromFile("../tests/1.txt");
	ContextFreeGrammar grammar1{grammar};
	grammar1.writeToFile("../tests/2.txt");
}
