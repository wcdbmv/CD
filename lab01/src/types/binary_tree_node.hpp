#pragma once

#include <functional>


template <typename T>
struct BinaryTreeNode {
	T data = T{};
	BinaryTreeNode* left = nullptr;
	BinaryTreeNode* right = nullptr;

	bool isLeaf() const {
		return !left && !right;
	}

	using Callback = std::function<void(BinaryTreeNode*)>;

	void applyToChildren(const Callback& callback) {
		for (auto child : {left, right}) {
			if (child) {
				callback(child);
			}
		}
	}

	static void clear(BinaryTreeNode* node) {
		node->applyToChildren(clear);
		delete node;
	}
};
