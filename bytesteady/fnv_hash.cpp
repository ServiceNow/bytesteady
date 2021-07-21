/*
 * Copyright 2021 ServiceNow
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *  http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
