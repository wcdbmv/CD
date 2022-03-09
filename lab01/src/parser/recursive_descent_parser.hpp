#pragma once

#include <parser/char_reader.hpp>
#include <types/abstract_syntax_tree_node.hpp>


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
