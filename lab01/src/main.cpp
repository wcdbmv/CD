#include <iostream>

#include <deterministic_finite_automaton.hpp>
#include <graphviz.hpp>


int main() {
	//DeterministicFiniteAutomaton dfa("(a|b)*abb");
	DeterministicFiniteAutomaton dfa("(a|b)*abb**(aa|b*a*|ab*)*");
	std::cout << generateLinkToGraphvizOnline(dfa.convertAstToDotFormat()) << std::endl;
	std::cout << generateLinkToGraphvizOnline(dfa.convertDfaToDotFormat()) << std::endl;
}
