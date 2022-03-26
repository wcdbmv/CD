#include <iostream>

#include <grammars/context_free_grammar.hpp>
#include <graph/graph.hpp>


namespace {

using Callback = std::function<void(ContextFreeGrammar&)>;
using CallbackRef = const Callback&;

bool sTest(
	const std::filesystem::path& grammar_path,
	const std::filesystem::path& result_path,
	const std::filesystem::path& expected_path,
	CallbackRef callback
) {
	ContextFreeGrammar grammar{Grammar::readFromFile(grammar_path)};
	const ContextFreeGrammar expected{Grammar::readFromFile(expected_path)};

	callback(grammar);

	grammar.writeToFile(result_path);

	return grammar == expected;
}

bool sEliminateLeftRecursionTest(size_t i) {
	const auto filepath = [i](const std::string& name) {
		return "../tests/ELR-" + std::to_string(i) + "-" + name + ".txt";
	};
	return sTest(filepath("grammar"), filepath("eliminated"), filepath("expected"), [](ContextFreeGrammar& grammar) {
		grammar.eliminateLeftRecursion();
	});
}

bool sGreibachNormalFormTest(size_t i) {
	const auto filepath = [i](const std::string& name) {
		return "../tests/GNF-" + std::to_string(i) + "-" + name + ".txt";
	};
	return sTest(filepath("grammar"), filepath("normalized"), filepath("expected"), [](ContextFreeGrammar& grammar) {
		grammar.greibachNormalForm();
	});
}

}  // namespace


int main() {
	std::cout << "Test ELR-1: " << (sEliminateLeftRecursionTest(1) ? "passed" : "failed") << std::endl;
	std::cout << "Test ELR-2: " << (sEliminateLeftRecursionTest(2) ? "passed" : "failed") << std::endl;
	std::cout << "Test GNF-1: " << (sGreibachNormalFormTest(1) ? "passed" : "failed") << std::endl;

	for (size_t i = 1; i <= 4; ++i) {
		ContextFreeGrammar grammar{Grammar::readFromFile("../tests/" + std::to_string(i) + ".txt")};
		grammar.eliminateLeftRecursion();
		grammar.writeToFile("../tests/" + std::to_string(i) + "-.txt");
	}
}
