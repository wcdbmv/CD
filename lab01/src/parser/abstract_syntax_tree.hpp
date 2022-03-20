#pragma once

#include <cstddef>

#include <types/abstract_syntax_tree_node.hpp>
#include <types/common.hpp>


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

	AbstractSyntaxTreeNode* findLeaf(char data);

	std::string toDotFormat() const;

private:
	AbstractSyntaxTreeNode* root_;

	UMap<AbstractSyntaxTreeNode*, size_t> leaf_to_index_;  // TODO: delete indices and use pointers
	UMap<size_t, AbstractSyntaxTreeNode*> index_to_leaf_;
	UMap<AbstractSyntaxTreeNode*, bool> nullable_;
	UMap<AbstractSyntaxTreeNode*, Set<size_t>> first_pos_;
	UMap<AbstractSyntaxTreeNode*, Set<size_t>> last_pos_;
	UMap<AbstractSyntaxTreeNode*, Set<size_t>> follow_pos_;

	void numerateLeaves_();
	void calculateNullable_();
	void calculateFirstPos_();
	void calculateLastPos_();
	void calculateFollowPos_();
};
