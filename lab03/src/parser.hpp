#pragma once

#include <list>
#include <optional>
#include <string>
#include <string_view>
#include <utility>


namespace Parser {

struct Node {
	std::string data;
	std::list<Node> children;
};

using ReturnType = std::pair<std::optional<Node>, std::string_view>;

ReturnType accept(std::string str);

}  // namespace Parser
