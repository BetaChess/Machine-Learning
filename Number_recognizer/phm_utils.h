#ifndef PHM_UTILS_H
#define PHM_UTILS_H

#include "phm_device.h"

#include <functional>

namespace phm {

	// from: https://stackoverflow.com/a/57595105
	template <typename T, typename... Rest>
	void hashCombine(std::size_t& seed, const T& v, const Rest&... rest) {
		seed ^= std::hash<T>{}(v)+0x9e3779b9 + (seed << 6) + (seed >> 2);
		(hashCombine(seed, rest), ...);
	};

	uint32_t findMemoryType(const PhmDevice& device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

}

#endif // PHM_UITLS_H