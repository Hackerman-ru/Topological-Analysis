#ifndef TOPA_BASIC_TYPES_HPP_
#define TOPA_BASIC_TYPES_HPP_

#include <cstdint>
#include <map>
#include <span>
#include <vector>
#include <unordered_set>

namespace topa::basic_types {

using DefaultId = int32_t;
using DefaultFloat = float;
using DefaultDistance = float;

template <typename T>
using DefaultView = std::span<T>;

template <typename T>
using DefaultContainer = std::vector<T>;

template <typename T>
using DefaultUnorderedSet = std::unordered_set<T>;

template <typename K, typename V>
using DefaultMap = std::map<K, V>;

}  // namespace topa::basic_types

#endif
