#pragma once

#include <algorithm>
#include <experimental/iterator>
#include <ostream>
#include <set>
#include <sstream>


template <typename T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& set) {
	os << "{";
	std::copy(set.begin(), set.end(), std::experimental::make_ostream_joiner(os, ", "));
	os << "}";
	return os;
}


namespace SetUtils {

template <typename T>
void append(std::set<T>& a, const std::set<T>& b) {
	a.insert(b.begin(), b.end());
}

template <typename T>
std::set<T> Union(const std::set<T>& a, const std::set<T>& b) {
	auto result = a;
	append(result, b);
	return result;
}

template <typename T>
auto intersection(const std::set<T>& a, const std::set<T>& b) {
	std::set<T> result;
	std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.begin()));
	return result;
}

template <typename T>
auto difference(const std::set<T>& a, const std::set<T>& b) {
	std::set<T> result;
	std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(result, result.end()));
	return result;
}

template <typename T>
auto popFirst(std::set<T>& set) {
	auto result = *set.begin();
	set.erase(set.begin());
	return result;
}

template <typename T>
std::string toString(const std::set<T>& set) {
	std::stringstream ss;
	ss << set;
	return ss.str();
}

}  // namespace SetUtils
