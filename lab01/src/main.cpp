#include <iostream>

#include <abstract_syntax_tree.hpp>
#include <graphviz.hpp>
#include <recursive_descent_parser.hpp>

template <typename T>
void sPrint(BinaryTreeNode<T>* root, int i = 0) {
	if (root) {
		auto indent = (i + 1) / 2;
		for (int j = 0; j < indent; ++j) {
			std::cout << "--";
		}
		std::cout << i << " " << root->data << std::endl;
		sPrint(root->left, 2 * i + 1);
		sPrint(root->right, 2 * i + 2);
	}
}


int main() {
	RecursiveDescentParser parser;
	auto* root = parser.parse("((a|b)*abb)#");
	std::cout << generateLinkToGraphvizOnline(root);
	AbstractSyntaxTree ast(root);

	AbstractSyntaxTreeNode::clear(root);
	return 0;
}