#pragma once

#include <algorithm>
#include <set>


void setAppend(auto& a, const auto& b) {
	a.insert(b.begin(), b.end());
}

auto setUnion(const auto& a, const auto& b) {
	auto result = a;
	setAppend(result, b);
	return result;
}

auto setIntersection(const auto& a, const auto& b) {
	auto result = a;
	std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.begin()));
	return result;
}
