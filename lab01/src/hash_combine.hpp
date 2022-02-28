#pragma once

#include <cstddef>


void hash_combine(size_t& seed, size_t value);

size_t hash_container(const auto& container) {
	size_t seed = container.size();
	for (auto&& value : container) {
		hash_combine(seed, value);
	}
	return seed;
}
