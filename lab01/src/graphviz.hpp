#pragma once

#include <string>

#include <abstract_syntax_tree_node.hpp>


std::string convertToDotFormat(AbstractSyntaxTreeNode* root);
std::string generateLinkToGraphvizOnline(AbstractSyntaxTreeNode* root);
