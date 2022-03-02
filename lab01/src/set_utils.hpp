#pragma once

#include <algorithm>
#include <type_traits>


void setAppend(auto& a, const auto& b) {
	a.insert(b.begin(), b.end());
}

auto setUnion(const auto& a, const auto& b) {
	auto result = a;
	setAppend(result, b);
	return result;
}

auto setIntersection(const auto& a, const auto& b) {
	std::remove_const_t<std::remove_reference_t<decltype(a)>> result;
	std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.begin()));
	return result;
}

auto setDifference(const auto& a, const auto& b) {
	std::remove_const_t<std::remove_reference_t<decltype(a)>> result;
	std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.end()));
	return result;
}

auto popFirst(auto& set) {
	auto result = *set.begin();
	set.erase(set.begin());
	return result;
}
