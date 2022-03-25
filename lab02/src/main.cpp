#include <iostream>

#include <grammar.hpp>


int main() {
	/*Grammar grammar{
		{"a", "b", "c"},
		{"A", "B", "C"},
		{
			{{"A"}, {"A", "a"}},
			{{"B"}, {"b", "B"}},
			{{"C", "a"}, {"a"}},
		},
		"A",
	};*/

	auto grammar = Grammar::readFromFile("../tests/1.txt");
	grammar.writeToFile("../tests/2.txt");
}
