#pragma once


template <typename T>
struct BinaryTreeNode {
	T data = T{};
	BinaryTreeNode* left = nullptr;
	BinaryTreeNode* right = nullptr;

	bool isLeaf() const {
		return !left && !right;
	}
};
