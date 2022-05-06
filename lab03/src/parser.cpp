#include <parser.hpp>

#include <algorithm>
#include <array>
#include <iostream>


namespace {

using namespace std::string_view_literals;

constexpr std::array kRelationalOperators = {"=="sv, "<>"sv, "<="sv, ">="sv, "<"sv, ">"sv};
constexpr std::array kSigns = {"+"sv, "-"sv};
constexpr std::array kAdditionOperators{"+"sv, "-"sv, "or"sv};
constexpr std::array kMultiplicationOperators{"*"sv, "/"sv, "div"sv, "mod"sv, "and"sv};
constexpr std::array kIdentifiers{"a"sv, "b"sv, "c"sv};
constexpr std::array kConstants{"const"sv};

using Parser::Node;
using Parser::ReturnType;

struct GrammarElement {
	virtual ReturnType accept(std::string_view str, size_t depth) = 0;
};

template <typename T>
ReturnType accept(std::string_view str, size_t depth)
{
	if constexpr (std::derived_from<T, GrammarElement>) {
		return T{}.accept(str, depth);
	} else {
		return {};
	}
}

struct Constant;
struct Expression1;
struct Expression;
struct Factor;
struct Identifier;
struct Operator;
struct OperatorsList;
struct Sign;
struct SimpleExpression1;
struct SimpleExpression;
struct Tail;
struct Term1;
struct Term;

constexpr bool firstIs(std::string_view str, char c) {
	return !str.empty() && str.front() == c;
}

struct Block : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Block: " << str << std::endl;

		Node tree{"Block", {}};
		if (firstIs(str, '{') && str.back() == '}') {
			if (auto&& [node, sv] = ::accept<OperatorsList>(str.substr(1), depth + 1); node) {
				if (firstIs(sv, '}')) {
					tree.children.push_back({"{", {}});
					tree.children.push_back(std::move(*node));
					tree.children.push_back({"}", {}});
					return {tree, sv.substr(1)};
				}
			}
		}

		return {std::nullopt, str};
	}
};

struct OperatorsList : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "OperatorList: " << str << std::endl;

		Node tree{"OperatorsList", {}};
		if (auto&& [node1, sv1] = ::accept<Operator>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<Tail>(sv1, depth + 2); node2) {
				tree.children.push_back(std::move(*node1));
				tree.children.push_back(std::move(*node2));
				return {tree, sv2};
			}
		}

		return {std::nullopt, str};
	}
};

struct Tail : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Tail: " << str << std::endl;

		Node tree{"Tail", {}};
		if (firstIs(str, ';')) {
			if (auto&& [node1, sv1] = ::accept<Operator>(str.substr(1), depth + 1); node1) {
				if (auto&& [node2, sv2] = ::accept<Tail>(sv1, depth + 2); node2) {
					tree.children.push_back({";", {}});
					tree.children.push_back(std::move(*node1));
					tree.children.push_back(std::move(*node2));
					return {tree, sv2};
				}
			}
		}

		tree.children.push_back({"ε", {}});
		return {tree, str};
	}
};

struct Operator : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Operator: " << str << std::endl;

		Node tree{"Operator", {}};
		if (auto&& [node1, sv1] = ::accept<Identifier>(str, depth + 1); node1) {
			if (firstIs(sv1, '=')) {
				if (auto&& [node2, sv2] = ::accept<Expression>(sv1.substr(1), depth + 2); node2) {
					tree.children.push_back(std::move(*node1));
					tree.children.push_back({"=", {}});
					tree.children.push_back(std::move(*node2));
					return {tree, sv2};
				}
			}
		}

		if (auto&& [node, sv] = ::accept<Block>(str, depth + 1); node) {
			tree.children.push_back(std::move(*node));
			return {tree, sv};
		}

		return {std::nullopt, str};
	}
};

struct Expression : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Expression: " << str << std::endl;

		Node tree{"Expression", {}};
		if (auto&& [node1, sv1] = ::accept<SimpleExpression>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<Expression1>(sv1, depth + 2); node2) {
				tree.children.push_back(std::move(*node1));
				tree.children.push_back(std::move(*node2));
				return {tree, sv2};
			}
		}

		return {std::nullopt, str};
	}
};

struct SimpleExpression : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "SimpleExpression: " << str << std::endl;

		Node tree{"SimpleExpression", {}};
		if (auto&& [node1, sv1] = ::accept<Term>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<SimpleExpression1>(sv1, depth + 2); node2) {
				tree.children.push_back(std::move(*node1));
				tree.children.push_back(std::move(*node2));
				return {tree, sv2};
			}
		}

		if (auto&& [node1, sv1] = ::accept<Sign>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<Term>(sv1, depth + 2); node2) {
				if (auto&& [node3, sv3] = ::accept<SimpleExpression1>(sv2, depth + 3); node3) {
					tree.children.push_back(std::move(*node1));
					tree.children.push_back(std::move(*node2));
					tree.children.push_back(std::move(*node3));
					return {tree, sv3};
				}
			}
		}

		return {std::nullopt, str};
	}
};

struct Term : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Term: " << str << std::endl;

		Node tree{"Term", {}};
		if (auto&& [node1, sv1] = ::accept<Factor>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<Term1>(sv1, depth + 2); node2) {
				tree.children.push_back(std::move(*node1));
				tree.children.push_back(std::move(*node2));
				return {tree, sv2};
			}
		}

		return {std::nullopt, str};
	}
};

struct Factor : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Factor: " << str << std::endl;

		Node tree{"Factor", {}};
		if (auto&& [node, sv] = ::accept<Identifier>(str, depth + 1); node) {
			tree.children.push_back(std::move(*node));
			return {tree, sv};
		}
		if (auto&& [node, sv] = ::accept<Constant>(str, depth + 1); node) {
			tree.children.push_back(std::move(*node));
			return {tree, sv};
		}
		if (firstIs(str, '(')) {
			if (auto&& [node, sv] = ::accept<SimpleExpression>(str.substr(1), depth + 1); node) {
				if (firstIs(sv, ')')) {
					tree.children.push_back({"(", {}});
					tree.children.push_back(std::move(*node));
					tree.children.push_back({")", {}});
					return {tree, sv.substr(1)};
				}
			}
		}
		if (str.starts_with("not")) {
			if (auto&& [node, sv] = ::accept<Factor>(str.substr(3), depth + 1); node) {
				tree.children.push_back(std::move(*node));
				return {tree, sv};
			}
		}

		return {std::nullopt, str};
	}
};

struct RelationOperation : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "RelationOperation: " << str << std::endl;

		Node tree{"RelationOperation", {}};
		for (auto&& op: kRelationalOperators) {
			if (str.starts_with(op)) {
				tree.children.push_back({std::string(op), {}});
				return {tree, str.substr(op.size())};
			}
		}

		return {std::nullopt, str};
	}
};

struct Sign : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Sign: " << str << std::endl;

		Node tree{"Sign", {}};
		for (auto&& sign: kSigns) {
			if (str.starts_with(sign)) {
				tree.children.push_back({std::string(sign), {}});
				return {tree, str.substr(sign.size())};
			}
		}

		return {std::nullopt, str};
	}
};

struct AdditionOperation : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "AdditionOperation: " << str << std::endl;

		Node tree{"AdditionOperation", {}};
		for (auto&& op: kAdditionOperators) {
			if (str.starts_with(op)) {
				tree.children.push_back({std::string(op), {}});
				return {tree, str.substr(op.size())};
			}
		}

		return {std::nullopt, str};
	}
};

struct MultiplicationOperation : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "MultiplicationOperation: " << str << std::endl;

		Node tree{"MultiplicationOperation", {}};
		for (auto&& op: kMultiplicationOperators) {
			if (str.starts_with(op)) {
				tree.children.push_back({std::string(op), {}});
				return {tree, str.substr(op.size())};
			}
		}

		return {std::nullopt, str};
	}
};

struct Identifier : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Identifier: " << str << std::endl;

		Node tree{"Identifier", {}};
		for (auto&& identifier: kIdentifiers) {
			if (str.starts_with(identifier)) {
				for (auto constant : kConstants) {
					if (str.starts_with(constant)) {
						tree.data = "Constant";
						tree.children.push_back({std::string(constant), {}});
						return {tree, str.substr(constant.size())};
					}
				}

				tree.children.push_back({std::string(identifier), {}});
				return {tree, str.substr(identifier.size())};
			}
		}

		return {std::nullopt, str};
	}
};

struct Constant : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Constant: " << str << std::endl;

		Node tree{"Constant", {}};
		for (auto&& constant: kConstants) {
			if (str.starts_with(constant)) {
				for (auto identifier : kIdentifiers) {
					if (str.starts_with(identifier)) {
						tree.data = "Identifier";
						tree.children.push_back({std::string(identifier), {}});
						return {tree, str.substr(identifier.size())};
					}
				}

				tree.children.push_back({std::string(constant), {}});
				return {tree, str.substr(constant.size())};
			}
		}

		return {std::nullopt, str};
	}
};

struct SimpleExpression1 : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "SimpleExpression': " << str << std::endl;

		Node tree{"SimpleExpression'", {}};
		if (auto&& [node1, sv1] = ::accept<AdditionOperation>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<Term>(sv1, depth + 2); node2) {
				if (auto&& [node3, sv3] = ::accept<SimpleExpression1>(sv2, depth + 3); node3) {
					tree.children.push_back(std::move(*node1));
					tree.children.push_back(std::move(*node2));
					tree.children.push_back(std::move(*node3));
					return {tree, sv3};
				}
			}
		}

		tree.children.push_back({"ε", {}});
		return {tree, str};
	}
};

struct Term1 : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Term': " << str << std::endl;

		Node tree{"Term'", {}};
		if (auto&& [node1, sv1] = ::accept<MultiplicationOperation>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<Factor>(sv1, depth + 2); node2) {
				if (auto&& [node3, sv3] = ::accept<Term1>(sv2, depth + 3); node3) {
					tree.children.push_back(std::move(*node1));
					tree.children.push_back(std::move(*node2));
					tree.children.push_back(std::move(*node3));
					return {tree, sv3};
				}
			}
		}

		tree.children.push_back({"ε", {}});
		return {tree, str};
	}
};

struct Expression1 : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Expression': " << str << std::endl;

		Node tree{"Expression'", {}};
		if (auto&& [node1, sv1] = ::accept<RelationOperation>(str, depth + 1); node1) {
			if (auto&& [node2, sv2] = ::accept<SimpleExpression>(sv1, depth + 2); node2) {
				tree.children.push_back(std::move(*node1));
				tree.children.push_back(std::move(*node2));
				return {tree, sv2};
			}
		}

		tree.children.push_back({"ε", {}});
		return {tree, str};
	}
};

}  // namespace


namespace Parser {

ReturnType accept(std::string str) {
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	return ::accept<Block>(str, 0);
}

}  // namespace Parser