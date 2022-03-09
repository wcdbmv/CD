#pragma once

#include <types/abstract_syntax_tree_node.hpp>
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
