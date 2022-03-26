#include <iostream>

#include <grammars/context_free_grammar.hpp>


namespace {

bool sTest1() {
	// https://www.csd.uwo.ca/~mmorenom/CS447/Lectures/Syntax.html/node8.html

	ContextFreeGrammar grammar{
		Grammar{
			{"A", "5"},
			{"a", "b", "c", "d"},
			{
				{{"5"}, {{"A", "a"}, {"b"}}},
				{{"A"}, {{"A", "c"}, {"A", "a", "d"}, {"b", "d"}, kEpsilon}},
			},
			"5",
		}
	};

	grammar.eliminateLeftRecursion();

	return grammar == Grammar{
		{"A", "A'", "5"},
		{"a", "b", "c", "d"},
		{
			{{"5"}, {{"A", "a"}, {"b"}}},
			{{"A"}, {{"b", "d", "A'"}, {"A'"}}},
			{{"A'"}, {{"c", "A'"}, {"a", "d", "A'"}, kEpsilon}},
		},
		"5",
	};
}

bool sTest2() {
	// [2] стр. 254

	ContextFreeGrammar grammar{
		Grammar{
			{"E", "T", "_F"},
			{"(", ")", "*", "+", "id"},
			{
				{{"E"}, {{"E", "+", "T"}, {"T"}}},
				{{"T"}, {{"T", "*", "_F"}, {"_F"}}},
				{{"_F"}, {{"(", "E", ")"}, {"id"}}},
			},
			"E",
		}
	};

	grammar.eliminateLeftRecursion();

	return grammar == Grammar{
		{"E", "E'", "T", "T'", "_F"},
		{"(", ")", "*", "+", "id"},
		{
			{{"E"}, {{"T", "E'"}}},
			{{"E'"}, {{"+", "T", "E'"}, kEpsilon}},
			{{"T"}, {{"_F", "T'"}}},
			{{"T'"}, {{"*", "_F", "T'"}, kEpsilon}},
			{{"_F"}, {{"(", "E", ")"}, {"id"}}},
		},
		"E",
	};
}

}  // namespace


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

	std::cout << "Test 1 " << (sTest1() ? "passed" : "failed") << std::endl;
	std::cout << "Test 2 " << (sTest2() ? "passed" : "failed") << std::endl;

	auto grammar = Grammar::readFromFile("../tests/1.txt");
	ContextFreeGrammar grammar1{grammar};
	grammar1.eliminateLeftRecursion();
	grammar1.writeToFile("../tests/1-.txt");

	grammar = Grammar::readFromFile("../tests/2.txt");
	ContextFreeGrammar grammar2{grammar};
	grammar2.eliminateLeftRecursion();
	grammar2.writeToFile("../tests/2-.txt");

	grammar = Grammar::readFromFile("../tests/3.txt");
	ContextFreeGrammar grammar3{grammar};
	grammar3.eliminateLeftRecursion();
	grammar3.writeToFile("../tests/3-.txt");

	grammar = Grammar::readFromFile("../tests/4.txt");
	ContextFreeGrammar grammar4{grammar};
	grammar4.eliminateLeftRecursion();
	grammar4.writeToFile("../tests/4-.txt");
}
