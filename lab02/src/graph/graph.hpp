#pragma once

#include <types/graph.hpp>


class Graph {
public:
	explicit Graph(Vertices vertices = {}, Edges edges = {});

	Vector<Vertex> topologicalSort() const;

	const auto& vertices() const { return vertices_; }
	const auto& edges() const { return edges_; }

private:
	Vertices vertices_;
	Edges edges_;

	void checkVertex_(const Vertex& v) const;
};
