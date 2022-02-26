#include <abstract_syntax_tree.hpp>

#include <stdexcept>


namespace {

template <typename T>
void sSetAppend(std::set<T>& a, const std::set<T>& b) {
	a.insert(b.begin(), b.end());
}

template <typename T>
std::set<T> sSetUnion(const std::set<T>& a, const std::set<T>& b) {
	auto result = a;
	sSetAppend(result, b);
	return result;
}

}  // namespace


AbstractSyntaxTree::AbstractSyntaxTree(AbstractSyntaxTreeNode* root)
	: root_{root}
{
	if (!root_) {
		throw std::invalid_argument("[AbstractSyntaxTree::AbstractSyntaxTree] root is nullptr");
	}

	size_t i = 1;
	numerateLeaves_(root_, i);

	calculateNullable_(root_);
	calculateFirstPos_(root_);
	calculateLastPos_(root_);
	calculateFollowPos_(root_);
	return;
}

void AbstractSyntaxTree::numerateLeaves_(AbstractSyntaxTreeNode* node, size_t& i) {
	if (node->isLeaf()) {
		leaf_to_index_[node] = i;
		index_to_leaf_[i] = node;
		++i;
		return;
	}

	for (auto* child : {node->left, node->right}) {
		if (child) {
			numerateLeaves_(child, i);
		}
	}
}

void AbstractSyntaxTree::calculateNullable_(AbstractSyntaxTreeNode* node) {
	if (node->isLeaf()) {
		nullable_[node] = node->data == 'E';
		return;
	}

	for (auto* child : {node->left, node->right}) {
		if (child) {
			calculateNullable_(child);
		}
	}

	if (node->data == '|') {
		nullable_[node] = nullable_[node->left] || nullable_[node->right];
	} else if (node->data == '&') {
		nullable_[node] = nullable_[node->left] && nullable_[node->right];
	} else if (node->data == '*') {
		nullable_[node] = true;
	} else {
		throw std::invalid_argument("[AbstractSyntaxTree::calculateNullable_] Unexpected AbstractSyntaxTreeNode");
	}
}

void AbstractSyntaxTree::calculateFirstPos_(AbstractSyntaxTreeNode* node) {
	if (node->isLeaf()) {
		if (node->data == 'E') {
			first_pos_[node] = {};
		} else {
			first_pos_[node] = {leaf_to_index_[node]};
		}
		return;
	}

	for (auto* child : {node->left, node->right}) {
		if (child) {
			calculateFirstPos_(child);
		}
	}

	if (node->data == '|') {
		first_pos_[node] = sSetUnion(first_pos_[node->left], first_pos_[node->right]);
	} else if (node->data == '&') {
		if (nullable_[node->left]) {
			first_pos_[node] = sSetUnion(first_pos_[node->left], first_pos_[node->right]);
		} else {
			first_pos_[node] = first_pos_[node->left];
		}
	} else if (node->data == '*') {
		first_pos_[node] = first_pos_[node->left];
	} else {
		throw std::invalid_argument("[AbstractSyntaxTree::calculateFirstPos_] Unexpected AbstractSyntaxTreeNode");
	}
}

void AbstractSyntaxTree::calculateLastPos_(AbstractSyntaxTreeNode* node) {
	if (node->isLeaf()) {
		if (node->data == 'E') {
			last_pos_[node] = {};
		} else {
			last_pos_[node] = {leaf_to_index_[node]};
		}
		return;
	}

	for (auto* child : {node->left, node->right}) {
		if (child) {
			calculateLastPos_(child);
		}
	}

	if (node->data == '|') {
		last_pos_[node] = sSetUnion(last_pos_[node->left], last_pos_[node->right]);
	} else if (node->data == '&') {
		if (nullable_[node->right]) {
			last_pos_[node] = sSetUnion(last_pos_[node->left], last_pos_[node->right]);
		} else {
			last_pos_[node] = last_pos_[node->right];
		}
	} else if (node->data == '*') {
		last_pos_[node] = last_pos_[node->left];
	} else {
		throw std::invalid_argument("[AbstractSyntaxTree::calculateLastPos_] Unexpected AbstractSyntaxTreeNode");
	}
}

void AbstractSyntaxTree::calculateFollowPos_(AbstractSyntaxTreeNode* node) {
	for (auto* child : {node->left, node->right}) {
		if (child) {
			calculateFollowPos_(child);
		}
	}

	if (node->data == '&') {
		for (auto i : last_pos_[node->left]) {
			sSetAppend(follow_pos_[index_to_leaf_[i]], first_pos_[node->right]);
		}
	} else if (node->data == '*') {
		for (auto i : last_pos_[node]) {
			sSetAppend(follow_pos_[index_to_leaf_[i]], first_pos_[node]);
		}
	}
}
