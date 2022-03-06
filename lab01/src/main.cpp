#include <iostream>

#include <deterministic_finite_automaton.hpp>
#include <graphviz.hpp>
#include <recursive_descent_parser.hpp>


namespace {

DeterministicFiniteAutomaton sBrzozowski(const FiniteAutomaton& A) {
	auto fa = A.reversed();
	std::cout << "r(A):\n" << generateLinkToGraphvizOnline(fa.toDotFormat()) << std::endl;

	auto dfa = DeterministicFiniteAutomaton(fa);
	std::cout << "dr(A):\n" << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	dfa.rename();
	std::cout << "mdr(A):\n" << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	fa = dfa.reversed();
	std::cout << "rmdr(A):\n" << generateLinkToGraphvizOnline(fa.toDotFormat()) << std::endl;

	dfa = DeterministicFiniteAutomaton(fa);
	std::cout << "drmdr(A):\n" << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	dfa.rename();
	std::cout << "mdrmdr(A):\n" << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	return dfa;
}

FiniteAutomaton sExample1() {
	// from http://sovietov.com/txt/minfa/minfa.html

	return FiniteAutomaton{
		{"0", "1", "2", "3"},
		{'a', 'b'},
		{
			{"0", {{'a', {"1", "2"}}, {'b', {"2"}}}},
			{"1", {{'a', {"2"}}, {'b', {"3"}}}},
			{"2", {{'a', {"1", "2"}}, {'b', {"3"}}}},
		},
		"0",
		{"3"}
	};
}

FiniteAutomaton sExample2() {
	// Пример 3.21 из
	// АХО А. В, ЛАМ М. С., СЕТИ Р., УЛЬМАН Дж. Д. Компиляторы: принципы, технологии и инструменты. – М.: Вильямс, 2008.

	return FiniteAutomaton{
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
	};
}

}  // namespace


int main() {
	std::cout << "Note:\n\tr - reverse\n\td - determinize\n\tm - rename\n\n";

	std::string expression;
	std::cout << "Input regular expression\n>>> ";
	std::cin >> expression;

	auto root = RecursiveDescentParser{}.parse("(" + std::string{expression} + ")#");
	auto ast = AbstractSyntaxTree{root};
	std::cout << "AST:\n" << generateLinkToGraphvizOnline(ast.toDotFormat()) << std::endl;

	DeterministicFiniteAutomaton dfa(expression);
	std::cout << "A (DFA):\n" << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;

	std::string ans;
	std::cout << "Rename [Y/n]? ";
	std::getline(std::cin, ans);
	std::getline(std::cin, ans);
	if (ans.find('n') == std::string::npos) {
		dfa.rename();
		std::cout << "A = m(A):\n" << generateLinkToGraphvizOnline(dfa.toDotFormat()) << std::endl;
	}

	dfa = sBrzozowski(dfa);

	std::cout << "Input string (or \"exit\")" << std::endl;
	while (true) {
		std::cout << ">>> ";

		std::string s;
		std::getline(std::cin, s);

		if (s == "exit") {
			break;
		}

		std::cout << dfa.accept(s) << std::endl;
	}
}
