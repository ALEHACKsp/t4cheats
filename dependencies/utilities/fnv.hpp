#pragma once

#include <cstdint>

namespace fnv {
	constexpr std::uint32_t offset_basis = 0x811c9dc5;
	constexpr std::uint32_t prime = 0x1000193;

	constexpr std::uint32_t hash(const char* str, const std::uint32_t value = offset_basis) {
		return *str ? hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(prime)) : value;
	}

	constexpr std::uint32_t hashRuntime(const char* str) {
		std::uint32_t value = offset_basis;

		while (*str) {
			value ^= *str++;
			value *= prime;
		}
		return value;
	}
}