#pragma once

#include <string>

#include <types/common.hpp>


using Vertex = std::string;
using Vertices = Set<Vertex>;

using Edges = UMap<Vertex, Vertices>;
