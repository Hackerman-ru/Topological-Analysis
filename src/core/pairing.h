#ifndef __PAIRING_H__
#define __PAIRING_H__

#include "simplex.h"

#include <optional>
#include <unordered_map>
#include <vector>

using Representatives = std::vector<Simplex>;
using PersistencePairing = std::unordered_map<Simplex, std::pair<std::optional<Simplex>, Representatives>>;

PersistencePairing create_pairing(const std::vector<WeightedSimplex>& weighted_simplices);

#endif
