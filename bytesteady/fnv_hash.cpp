/*
 * Copyright 2018 Xiang Zhang All Rights Reserved.
 */

#include "bytesteady/fnv_hash.hpp"

#include <stdint.h>

static const uint64_t kPrime = 1099511628211ULL;
static const uint64_t kOffsetBasis = 0xcbf29ce484222325ULL;

namespace bytesteady{

// FNV-1a hash in 64 bit
uint64_t FNVHash::hash64(const uint8_t *s, uint64_t len, uint64_t seed) {
  uint64_t hash = kOffsetBasis;
  for (int64_t i = 0; i < len; ++i) {
    hash = hash ^ static_cast< uint64_t >(s[i]);
    hash = hash * kPrime;
  }
  return hash;
}

}  // namespace bytesteady
