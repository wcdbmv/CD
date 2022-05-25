#include <graphviz.hpp>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>


namespace {

struct Vertex {
	size_t id{};
	std::string label;

	bool operator<=>(const Vertex&) const = default;
};

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, Vertex>;

void sGenerateDot(const Parser::Node& tree, const std::filesystem::path& path) {
	Graph graph;

	const auto vertex = [&graph](size_t id, const std::string& name) {
		for (auto&& v : boost::make_iterator_range(vertices(graph))) {
			if (graph[v] == Vertex{id, name}) {
				return v;
			}
		}
		return add_vertex({id, name}, graph);
	};

	size_t count = 0;
	std::stack<std::pair<Parser::Node, size_t>> stack;
	stack.push({tree, count});

	while (!stack.empty()) {
		auto [n, num] = stack.top();
		stack.pop();

		auto u = vertex(num, n.data + " " + std::to_string(num));

		for (auto&& child : n.children) {
			++count;
			stack.push({child, count});

			auto v = vertex(count, child.data + " " + std::to_string(count));
			boost::add_edge(u, v, graph);
		}
	}

	boost::dynamic_properties dp;
	dp.property("node_id", boost::get(&Vertex::id, graph));
	dp.property("label", boost::get(&Vertex::label, graph));

	// write to dot
	std::ofstream file{path};
	write_graphviz_dp(file, graph, dp);
	write_graphviz(file, graph);
}

std::string sReadFile(const std::filesystem::path& path) {
	std::ifstream t{path};
	t.seekg(0, std::ios::end);
	auto size = t.tellg();
	std::string buffer(size, ' ');
	t.seekg(0);
	t.read(&buffer[0], size);
	return buffer;
}

std::string sGenerateLinkToGraphvizOnline(std::string_view dot) {
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


}  // namespace


namespace Graphviz {

void saveDot(const Parser::Node& tree, const std::filesystem::path& path) {
	sGenerateDot(tree, path);
}

std::string generateLinkToGraphvizOnline(const std::filesystem::path& path) {
	const auto dot = sReadFile(path);
	return sGenerateLinkToGraphvizOnline(dot);
}

}  // namespace Graphviz
