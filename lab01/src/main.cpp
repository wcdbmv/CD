#include <iostream>

#include <deterministic_finite_automaton.hpp>
#include <graphviz.hpp>


int main() {
	//DeterministicFiniteAutomaton dfa("(a|b)*abb");
	//DeterministicFiniteAutomaton dfa("(a|b)*abb**(aa|b*a*|ab*)*");
	FiniteAutomaton fa(
		{"0", "1", "2", "3"},
		{'a', 'b'},
		{
			{"0", {{'a', {"1", "2"}}, {'b', {"2"}}}},
			{"1", {{'a', {"2"}}, {'b', {"3"}}}},
			{"2", {{'a', {"1", "2"}}, {'b', {"3"}}}},
		},
		"0",
		{"3"}
		);
	/*FiniteAutomaton fa(
		{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"},
		{'a', 'b'},
		{
			{"0", {{'L', {"1", "7"}}}},
			{"1", {{'L', {"2", "4"}}}},
			{"2", {{'a', {"3"}}}},
			{"3", {{'L', {"6"}}}},
			{"4", {{'b', {"5"}}}},
			{"5", {{'L', {"6"}}}},
			{"6", {{'L', {"1", "7"}}}},
			{"7", {{'a', {"8"}}}},
			{"8", {{'b', {"9"}}}},
			{"9", {{'b', {"10"}}}},
		},
		"0",
		{"10"}
	);*/
	std::cout << generateLinkToGraphvizOnline(fa.toDotFormat()) << std::endl;

	fa.reverse();
	std::cout << generateLinkToGraphvizOnline(fa.toDotFormat()) << std::endl;

	auto dfa = DeterministicFiniteAutomaton(fa);
	std::cout << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	dfa.rename();
	std::cout << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	fa = dfa.reversed();
	std::cout << generateLinkToGraphvizOnline(fa.toDotFormat()) << std::endl;

	dfa = DeterministicFiniteAutomaton(fa);
	std::cout << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	dfa.rename();
	std::cout << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;
}
