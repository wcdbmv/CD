#include <parser.hpp>

#include <algorithm>
#include <array>
#include <iostream>
#include <span>


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
		return {std::nullopt, str};
	}
}

struct Constant;
struct Expression;
struct Expression1;
struct Factor;
struct Identifier;
struct Operator;
struct OperatorsList;
struct Sign;
struct SimpleExpression;
struct SimpleExpression1;
struct Tail;
struct Term;
struct Term1;

template <char C>
struct Char : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		std::cout << std::string(depth, '\t') << "Char<'" << C << "'>: " << str << std::endl;

		Node tree{std::string{C}, {}};
		if (str.starts_with(C)) {
			return {tree, str.substr(1)};
		}

		return {std::nullopt, str};
	}
};

using LeftCurlyBracket = Char<'{'>;
using RightCurlyBracket = Char<'}'>;
using Semicolon = Char<';'>;
using Assign = Char<'='>;
using LeftParenthesis = Char<'('>;
using RightParenthesis = Char<')'>;

// Yes, I lost my mind

#define ACCEPT1(T) \
	if (auto&& [node, sv] = ::accept<T>(str, depth + 1); node) { \
		tree.children.push_back(std::move(*node)); \
		return {tree, sv}; \
	}

#define ACCEPT2(T1, T2) \
	if (auto&& [node1, sv1] = ::accept<T1>(str, depth + 1); node1) { \
		if (auto&& [node2, sv2] = ::accept<T2>(sv1, depth + 2); node2) { \
			tree.children.push_back(std::move(*node1)); \
			tree.children.push_back(std::move(*node2)); \
			return {tree, sv2}; \
		} \
	}

#define ACCEPT3(T1, T2, T3) \
	if (auto&& [node1, sv1] = ::accept<T1>(str, depth + 1); node1) { \
		if (auto&& [node2, sv2] = ::accept<T2>(sv1, depth + 2); node2) { \
			if (auto&& [node3, sv3] = ::accept<T3>(sv2, depth + 3); node3) { \
				tree.children.push_back(std::move(*node1)); \
				tree.children.push_back(std::move(*node2)); \
				tree.children.push_back(std::move(*node3)); \
				return {tree, sv3}; \
			} \
		} \
	}

#define TREE(label) \
	std::cout << std::string(depth, '\t') << label << ": " << str << std::endl; \
	Node tree{label, {}};

#define RETURN_ERROR \
	return {std::nullopt, str};

#define RETURN_EPS \
	tree.children.push_back({"ε", {}}); \
	return {tree, str};

#define RETURN_EPS_OR_ERROR \
	if constexpr (Eps) { \
		RETURN_EPS \
	} else { \
		RETURN_ERROR \
	}

template <typename T1, typename T2, bool Eps = false>
ReturnType accept(const std::string& label, std::string_view str, size_t depth) {
	TREE(label)
	ACCEPT2(T1, T2)
	RETURN_EPS_OR_ERROR
}

template <typename T1, typename T2, typename T3, bool Eps = false>
ReturnType accept(const std::string& label, std::string_view str, size_t depth) {
	TREE(label)
	ACCEPT3(T1, T2, T3)
	RETURN_EPS_OR_ERROR
}

#define GRAMMAR_ELEMENT_FROM_SINGLE_RULE_BASE(name, label, ...) \
	struct name : GrammarElement { \
		ReturnType accept(std::string_view str, size_t depth) override { \
			return ::accept<__VA_ARGS__>(label, str, depth); \
		} \
	};

#define GRAMMAR_ELEMENT_FROM_SINGLE_RULE(name, ...) GRAMMAR_ELEMENT_FROM_SINGLE_RULE_BASE(name, #name, __VA_ARGS__)

GRAMMAR_ELEMENT_FROM_SINGLE_RULE(Block, LeftCurlyBracket, OperatorsList, RightCurlyBracket)
GRAMMAR_ELEMENT_FROM_SINGLE_RULE(OperatorsList, Operator, Tail)
GRAMMAR_ELEMENT_FROM_SINGLE_RULE(Tail, Semicolon, Operator, Tail, true)

struct Operator : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		TREE("Operator")
		ACCEPT3(Identifier, Assign, Expression)
		ACCEPT1(Block)
		RETURN_ERROR
	}
};

GRAMMAR_ELEMENT_FROM_SINGLE_RULE(Expression, SimpleExpression, Expression1)

struct SimpleExpression : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		TREE("SimpleExpression")
		ACCEPT2(Term, SimpleExpression1)
		ACCEPT3(Sign, Term, SimpleExpression1)
		RETURN_ERROR
	}
};

GRAMMAR_ELEMENT_FROM_SINGLE_RULE(Term, Factor, Term1)

struct Factor : GrammarElement {
	ReturnType accept(std::string_view str, size_t depth) override {
		TREE("Factor")
		ACCEPT1(Constant)
		ACCEPT1(Identifier)
		ACCEPT3(LeftParenthesis, SimpleExpression, RightParenthesis)
		if (str.starts_with("not")) {
			if (auto&& [node, sv] = ::accept<Factor>(str.substr(3), depth + 1); node) {
				tree.children.push_back(std::move(*node));
				return {tree, sv};
			}
		}
		RETURN_ERROR
	}
};

ReturnType sAccept(const std::string& label, std::span<const std::string_view> array, std::string_view str, size_t depth) {
	TREE(label)
	for (auto&& item: array) {
		if (str.starts_with(item)) {
			tree.children.push_back({std::string(item), {}});
			return {tree, str.substr(item.size())};
		}
	}
	RETURN_ERROR
}

#define GRAMMAR_ELEMENT_FROM_ARRAY(name, array) \
	struct name : GrammarElement { \
		ReturnType accept(std::string_view str, size_t depth) override { \
			return sAccept(#name, array, str, depth); \
		} \
	};

GRAMMAR_ELEMENT_FROM_ARRAY(RelationOperation, kRelationalOperators)
GRAMMAR_ELEMENT_FROM_ARRAY(Sign, kSigns)
GRAMMAR_ELEMENT_FROM_ARRAY(AdditionOperation, kAdditionOperators)
GRAMMAR_ELEMENT_FROM_ARRAY(MultiplicationOperation, kMultiplicationOperators)
GRAMMAR_ELEMENT_FROM_ARRAY(Identifier, kIdentifiers)
GRAMMAR_ELEMENT_FROM_ARRAY(Constant, kConstants)

GRAMMAR_ELEMENT_FROM_SINGLE_RULE_BASE(SimpleExpression1, "SimpleExpression'", AdditionOperation, Term, SimpleExpression1, true)
GRAMMAR_ELEMENT_FROM_SINGLE_RULE_BASE(Term1, "Term'", MultiplicationOperation, Factor, Term1, true)
GRAMMAR_ELEMENT_FROM_SINGLE_RULE_BASE(Expression1, "Expression'", RelationOperation, SimpleExpression, true)

}  // namespace


namespace Parser {

ReturnType accept(std::string str) {
	str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	return ::accept<Block>(str, 0);
}

}  // namespace Parser
