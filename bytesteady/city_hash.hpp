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
