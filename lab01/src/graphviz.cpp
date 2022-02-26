#include <graphviz.hpp>

#include <iomanip>


namespace {

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


std::string convertToDotFormat(AbstractSyntaxTreeNode* root) {
	return "graph AST {\n" + sConvertToDotFormatInternal(root) + "}\n";
};

std::string generateLinkToGraphvizOnline(AbstractSyntaxTreeNode* root) {
	const auto raw = convertToDotFormat(root);
	std::string result = "https://dreampuf.github.io/GraphvizOnline/#";
	for (auto c : raw) {
		switch (c) {
		case '\t':
			result += "%09";
			break;
		case '\n':
			result += "%0A";
			break;
		case '#':
			result += "%23";
			break;
		default:
			result += c;
			break;
		}
	}
	return result;
}
