#pragma once

#include <cstddef>
#include <set>
#include <string>
#include <unordered_map>

#include <abstract_syntax_tree_node.hpp>


class AbstractSyntaxTree {
public:
	explicit AbstractSyntaxTree(AbstractSyntaxTreeNode* root);
	~AbstractSyntaxTree();

	auto& root() { return root_; }
	auto& leafToIndex() { return leaf_to_index_; };
	auto& indexToLeaf() { return index_to_leaf_; };
	auto& nullable() { return nullable_; }
	auto& firstPos() { return first_pos_; }
	auto& lastPos() { return last_pos_; }
	auto& followPos() { return follow_pos_; }

	std::string convertToDotFormat() const;

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
