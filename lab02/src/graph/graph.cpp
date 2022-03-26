#include <graph/graph.hpp>

#include <stdexcept>


Graph::Graph(Vertices vertices, Edges edges)
	: vertices_{std::move(vertices)}
	, edges_{std::move(edges)}
{
	for (auto&& [from, tos] : edges_) {
		checkVertex_(from);
		for (auto&& to : tos) {
			checkVertex_(to);
		}
	}
}

void Graph::checkVertex_(const Vertex& v) const {
	if (!vertices_.contains(v)) {
		throw std::invalid_argument("[Graph::checkVertex_] \"" + v + "\" is not in V");
	}
}

Vector<Vertex> Graph::topologicalSort() const {
	UMap<Vertex, bool> used;
	for (auto&& v : vertices_) {
		used[v] = false;
	}

	Vector<Vertex> order;

	std::function<void(const Vertex& v)> dfs;
	dfs = [&](const Vertex& v) {
		used[v] = true;
		if (auto adj = edges_.find(v); adj != edges_.end()) {
			for (auto&& u : adj->second) {
				if (!used[u]) {
					dfs(u);
				}
			}
		}
		order.push_back(v);
	};

	for (auto&& v : vertices_) {
		if (!used[v]) {
			dfs(v);
		}
	}

	std::reverse(order.begin(), order.end());

	return order;
}
