#pragma once

#include <functional>
#include <set>
#include <unordered_map>
#include <vector>


template <typename T>
using Set = std::set<T>;

template <typename Key, typename T>
using UMap = std::unordered_map<Key, T>;

template <typename T>
using Vector = std::vector<T>;

template <typename T>
struct std::hash<Vector<T>> {
	std::size_t operator()(const Vector<T>& vector) const noexcept {
		std::size_t hash = std::hash<size_t>{}(vector.size());
		for (auto&& i : vector) {
			hash ^= std::hash<T>{}(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
		}
		return hash;
	}
};
