#include <parser/recursive_descent_parser.hpp>


AbstractSyntaxTreeNode* RecursiveDescentParser::parse(std::string_view expression) {
	load_(expression);
	return regex_();
}

AbstractSyntaxTreeNode* RecursiveDescentParser::regex_() {
	auto* term = term_();

	if (!empty_() && peek_() == '|') {
		eat_();  // '|'
		auto* regex = regex_();
		return new AbstractSyntaxTreeNode{'|', term, regex};
	}

	return term;
}

AbstractSyntaxTreeNode* RecursiveDescentParser::term_() {
	if (empty_() || peek_() == ')' || peek_() == '|') {
		return new AbstractSyntaxTreeNode{'E'};
	}

	auto* factor = factor_();

	while (!empty_() && peek_() != ')' && peek_() != '|') {
		auto* next_factor = factor_();
		factor = new AbstractSyntaxTreeNode{'&', factor, next_factor};
	}

	return factor;
}

AbstractSyntaxTreeNode* RecursiveDescentParser::factor_() {
	auto* base = base_();

	while (!empty_() && peek_() == '*') {
		eat_();  // '*';
		base = new AbstractSyntaxTreeNode{'*', base};
	}

	return base;
}

AbstractSyntaxTreeNode* RecursiveDescentParser::base_() {
	if (peek_() == '(') {
		eat_();  // '('
		auto* regex = regex_();
		eat_(')');
		return regex;
	}

	return new AbstractSyntaxTreeNode{next_()};
}
