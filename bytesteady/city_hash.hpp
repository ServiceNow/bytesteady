/*
 * Copyright 2018 Xiang Zhang All Rights Reserved.
 */

#ifndef BYTESTEADY_CITY_HASH_HPP_
#define BYTESTEADY_CITY_HASH_HPP_

#include "bytesteady/integer.hpp"

namespace bytesteady {

class CityHash {
 public:
  static uint64_t hash64(const uint8_t *s, uint64_t len, uint64_t seed);
};

}  // namespace bytesteady

#endif  // BYTESTEADY_CITY_HASH_HPP_
