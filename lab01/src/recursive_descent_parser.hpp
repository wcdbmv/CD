#pragma once

#include <string_view>

#include <abstract_syntax_tree_node.hpp>


#include <iostream>

#include <char_reader.hpp>


class RecursiveDescentParser : public CharReader
{
public:
	AbstractSyntaxTreeNode* parse(std::string_view expression);

private:
	AbstractSyntaxTreeNode* regex_();
	AbstractSyntaxTreeNode* term_();
	AbstractSyntaxTreeNode* factor_();
	AbstractSyntaxTreeNode* base_();
};
