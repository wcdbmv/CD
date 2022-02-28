#pragma once

#include <set>


template <typename T>
void set_append(std::set<T>& a, const std::set<T>& b) {
	a.insert(b.begin(), b.end());
}

template <typename T>
std::set<T> set_union(const std::set<T>& a, const std::set<T>& b) {
	auto result = a;
	set_append(result, b);
	return result;
}
