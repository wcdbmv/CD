#pragma once

#include <cstddef>
#include <set>
#include <unordered_map>

#include <abstract_syntax_tree_node.hpp>


class AbstractSyntaxTree {
public:
	explicit AbstractSyntaxTree(AbstractSyntaxTreeNode* root);

private:
	AbstractSyntaxTreeNode* root_;

	std::unordered_map<AbstractSyntaxTreeNode*, size_t> leaf_to_index_;  // TODO: delete indices and use pointers
	std::unordered_map<size_t, AbstractSyntaxTreeNode*> index_to_leaf_;
	std::unordered_map<AbstractSyntaxTreeNode*, bool> nullable_;
	std::unordered_map<AbstractSyntaxTreeNode*, std::set<size_t>> first_pos_;
	std::unordered_map<AbstractSyntaxTreeNode*, std::set<size_t>> last_pos_;
	std::unordered_map<AbstractSyntaxTreeNode*, std::set<size_t>> follow_pos_;

	void numerateLeaves_(AbstractSyntaxTreeNode* node, size_t& i);
	void calculateNullable_(AbstractSyntaxTreeNode* node);
	void calculateFirstPos_(AbstractSyntaxTreeNode* node);
	void calculateLastPos_(AbstractSyntaxTreeNode* node);
	void calculateFollowPos_(AbstractSyntaxTreeNode* node);
};
