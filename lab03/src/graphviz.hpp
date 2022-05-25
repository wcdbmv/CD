#pragma once

#include <filesystem>

#include <parser.hpp>


namespace Graphviz {

void saveDot(const Parser::Node& tree, const std::filesystem::path& path);
std::string generateLinkToGraphvizOnline(const std::filesystem::path& path);

}  // namespace Graphviz
