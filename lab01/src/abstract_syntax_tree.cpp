#include <abstract_syntax_tree.hpp>

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <set_utils.hpp>


namespace {

using CallbackRef = const AbstractSyntaxTreeNode::Callback&;

void sDummyCallback(AbstractSyntaxTreeNode*) {}

void sTraverse(AbstractSyntaxTreeNode* node,
               CallbackRef onLeaf = sDummyCallback,
               CallbackRef onChild = sDummyCallback,
               CallbackRef onOr = sDummyCallback,
               CallbackRef onCat = sDummyCallback,
               CallbackRef onStar = sDummyCallback) {

	if (node->isLeaf()) {
		onLeaf(node);
		return;
	}

	node->applyToChildren(onChild);

	if (node->data == '|') {
		onOr(node);
	} else if (node->data == '&') {
		onCat(node);
	} else if (node->data == '*') {
		onStar(node);
	} else {
		throw std::invalid_argument("[<anonymous namespace>::sTraverse] Unexpected AbstractSyntaxTreeNode");
	}
}

std::string sConvertToDotFormatInternal(AbstractSyntaxTreeNode* root) {
	if (!root) {
		return "";
	}

	std::stringstream stream;
	stream << "\t\"" << std::hex << reinterpret_cast<void*>(root) << "\" [label=\"" << root->data << "\"]\n";
	if (root->left || root->right) {
		stream << "\t\"" << std::hex << reinterpret_cast<void*>(root) << "\" -- {\n";
		for (auto* child : {root->left, root->right}) {
			if (child) {
				stream << "\t\t\"" << std::hex << reinterpret_cast<void*>(child) << "\"\n";
			}
		}
		stream << "\t}\n\n";
	}

	stream << sConvertToDotFormatInternal(root->left);
	stream << sConvertToDotFormatInternal(root->right);

	return stream.str();
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
}

AbstractSyntaxTree::~AbstractSyntaxTree() {
	AbstractSyntaxTreeNode::clear(root_);
}

void AbstractSyntaxTree::numerateLeaves_(AbstractSyntaxTreeNode* node, size_t& i) {
	sTraverse(
		node,
		[&](auto* node) {
			leaf_to_index_[node] = i;
			index_to_leaf_[i] = node;
			++i;
		},
		[&](auto* child) { numerateLeaves_(child, i); }
	);
}

void AbstractSyntaxTree::calculateNullable_(AbstractSyntaxTreeNode* node) {
	sTraverse(
		node,
		[&](auto* node) { nullable_[node] = node->data == 'E'; },
		[&](auto* child) { calculateNullable_(child); },
		[&](auto* node) { nullable_[node] = nullable_[node->left] || nullable_[node->right]; },
		[&](auto* node) { nullable_[node] = nullable_[node->left] && nullable_[node->right]; },
		[&](auto* node) { nullable_[node] = true; }
	);
}

void AbstractSyntaxTree::calculateFirstPos_(AbstractSyntaxTreeNode* node) {
	sTraverse(
		node,
		[&](auto* node) {
			if (node->data == 'E') {
				first_pos_[node] = {};
			} else {
				first_pos_[node] = {leaf_to_index_[node]};
			}
		},
		[&](auto* child) { calculateFirstPos_(child); },
		[&](auto* node) { first_pos_[node] = setUnion(first_pos_[node->left], first_pos_[node->right]); },
		[&](auto* node) {
			if (nullable_[node->left]) {
				first_pos_[node] = setUnion(first_pos_[node->left], first_pos_[node->right]);
			} else {
				first_pos_[node] = first_pos_[node->left];
			}
		},
		[&](auto* node) { first_pos_[node] = first_pos_[node->left]; }
	);
}

void AbstractSyntaxTree::calculateLastPos_(AbstractSyntaxTreeNode* node) {
	sTraverse(
		node,
		[&](auto* node) {
			if (node->data == 'E') {
				last_pos_[node] = {};
			} else {
				last_pos_[node] = {leaf_to_index_[node]};
			}
		},
		[&](auto* child) { calculateLastPos_(child); },
		[&](auto* node) { last_pos_[node] = setUnion(last_pos_[node->left], last_pos_[node->right]); },
		[&](auto* node) {
			if (nullable_[node->right]) {
				last_pos_[node] = setUnion(last_pos_[node->left], last_pos_[node->right]);
			} else {
				last_pos_[node] = last_pos_[node->right];
			}
		},
		[&](auto* node) { last_pos_[node] = last_pos_[node->left]; }
	);
}

void AbstractSyntaxTree::calculateFollowPos_(AbstractSyntaxTreeNode* node) {
	sTraverse(
		node,
		sDummyCallback,
		[&](auto* child) { calculateFollowPos_(child); },
		sDummyCallback,
		[&](auto* node) {
			for (auto i : last_pos_[node->left]) {
				setAppend(follow_pos_[index_to_leaf_[i]], first_pos_[node->right]);
			}
		},
		[&](auto* node) {
			for (auto i : last_pos_[node]) {
				setAppend(follow_pos_[index_to_leaf_[i]], first_pos_[node]);
			}
		}
	);
}

std::string AbstractSyntaxTree::convertToDotFormat() const {
	return "graph AST {\n" + sConvertToDotFormatInternal(root_) + "}\n";
}
