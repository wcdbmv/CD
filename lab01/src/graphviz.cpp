#include <graphviz.hpp>


std::string generateLinkToGraphvizOnline(std::string_view dot) {
	std::string result = "https://dreampuf.github.io/GraphvizOnline/#";
	for (auto c : dot) {
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
