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

#include "bytesteady/city_hash.hpp"

#include <byteswap.h>
#include <stdint.h>
#include <string.h>
#include <utility>

static const uint64_t k0 = 0xc3a5c85c97cb3127ULL;
static const uint64_t k1 = 0xb492b66fbe98f273ULL;
static const uint64_t k2 = 0x9ae16a3b2f90404fULL;
static const uint64_t k3 = 0x9ddfea08eb382d69ULL;

static uint64_t fetch64(const uint8_t *p) {
  uint64_t result;
  memcpy(&result, p, sizeof(result));
  return result;
}

static uint32_t fetch32(const uint8_t *p) {
  uint32_t result;
  memcpy(&result, p, sizeof(result));
  return result;
}

static uint64_t rotate(uint64_t val, uint64_t shift) {
  return shift == 0 ? val : ((val >> shift) | (val << (64 - shift)));
}

static uint64_t shiftMix(uint64_t val) {
  return val ^ (val >> 47);
}

static uint64_t hashLen16(uint64_t u, uint64_t v, uint64_t mul) {
  uint64_t a = (u ^ v) * mul;
  a ^= (a >> 47);
  uint64_t b = (v ^ a) * mul;
  b ^= (b >> 47);
  b *= mul;
  return b;
}

static uint64_t hashLen0to16(const uint8_t *s, uint64_t len) {
  if (len >= 8) {
    uint64_t mul = k2 + len * 2;
    uint64_t a = fetch64(s) + k2;
    uint64_t b = fetch64(s + len - 8);
    uint64_t c = rotate(b, 37) * mul + a;
    uint64_t d = (rotate(a, 25) + b) * mul;
    return hashLen16(c, d, mul);
  }
  if (len >= 4) {
    uint64_t mul = k2 + len * 2;
    uint64_t a = fetch32(s);
    return hashLen16(len + (a << 3), fetch32(s + len - 4), mul);
  }
  if (len > 0) {
    uint8_t a = s[0];
    uint8_t b = s[len >> 1];
    uint8_t c = s[len - 1];
    uint32_t y = static_cast< uint32_t >(a) + (static_cast< uint32_t >(b) << 8);
    uint32_t z = len + (static_cast< uint32_t >(c) << 2);
    return shiftMix(y * k2 ^ z * k0) * k2;
  }
  return k2;
}

static uint64_t hashLen17to32(const uint8_t *s, uint64_t len) {
  uint64_t mul = k2 + len * 2;
  uint64_t a = fetch64(s) * k1;
  uint64_t b = fetch64(s + 8);
  uint64_t c = fetch64(s + len - 8) * mul;
  uint64_t d = fetch64(s + len - 16) * k2;
  return hashLen16(rotate(a + b, 43) + rotate(c, 30) + d,
                   a + rotate(b + k2, 18) + c, mul);
}

static uint64_t hashLen33to64(const uint8_t *s, uint64_t len) {
  uint64_t mul = k2 + len * 2;
  uint64_t a = fetch64(s) * k2;
  uint64_t b = fetch64(s + 8);
  uint64_t c = fetch64(s + len - 24);
  uint64_t d = fetch64(s + len - 32);
  uint64_t e = fetch64(s + 16) * k2;
  uint64_t f = fetch64(s + 24) * 9;
  uint64_t g = fetch64(s + len - 8);
  uint64_t h = fetch64(s + len - 16) * mul;
  uint64_t u = rotate(a + g, 43) + (rotate(b, 30) + c) * 9;
  uint64_t v = ((a + g) ^ d) + f + 1;
  uint64_t w = bswap_64((u + v) * mul) + h;
  uint64_t x = rotate(e + f, 42) + c;
  uint64_t y = (bswap_64((v + w) * mul) + g) * mul;
  uint64_t z = e + f + c;
  a = bswap_64((x + z) * mul + y) + b;
  b = shiftMix((z + a) * mul + d + h) * mul;
  return b + x;
}

static ::std::pair<uint64_t, uint64_t> weakHashLen32WithSeeds(
    const uint8_t* s, uint64_t a, uint64_t b) {
  uint64_t w = fetch64(s);
  uint64_t x = fetch64(s + 8);
  uint64_t y = fetch64(s + 16);
  uint64_t z = fetch64(s + 24);
  a += w;
  b = rotate(b + a + z, 21);
  uint64_t c = a;
  a += x;
  a += y;
  b += rotate(a, 44);
  return ::std::make_pair(a + z, b + c);
}

static uint64_t cityHash64(const uint8_t *s, uint64_t len) {
  if (len <= 32) {
    if (len <= 16) {
      return hashLen0to16(s, len);
    } else {
      return hashLen17to32(s, len);
    }
  } else if (len <= 64) {
    return hashLen33to64(s, len);
  }

  // For strings over 64 bytes, hash the end first
  uint64_t x = fetch64(s + len - 40);
  uint64_t y = fetch64(s + len - 16) + fetch64(s + len - 56);
  uint64_t z = hashLen16(
      fetch64(s + len - 48) + len, fetch64(s + len - 24), k3);
  ::std::pair<uint64_t, uint64_t> v =
        weakHashLen32WithSeeds(s + len - 64, len, z);
  ::std::pair<uint64_t, uint64_t> w =
        weakHashLen32WithSeeds(s + len - 32, y + k1, x);
  x = x * k1 + fetch64(s);

  // Decrease len to the nearest multiple of 64, and operate on 64-byte chunks.
  len = (len - 1) & ~static_cast< uint64_t >(63);
  do {
    x = rotate(x + y + v.first + fetch64(s + 8), 37) * k1;
    y = rotate(y + v.second + fetch64(s + 48), 42) * k1;
    x ^= w.second;
    y += v.first + fetch64(s + 40);
    z = rotate(z + w.first, 33) * k1;
    v = weakHashLen32WithSeeds(s, v.second * k1, x + w.first);
    w = weakHashLen32WithSeeds(s + 32, z + w.second, y + fetch64(s + 16));
    ::std::swap(z, x);
    s += 64;
    len -= 64;
  } while (len != 0);
  return hashLen16(hashLen16(v.first, w.first, k3) + shiftMix(y) * k1 + z,
                   hashLen16(v.second, w.second, k3) + x, k3);
}

namespace bytesteady{

uint64_t CityHash::hash64(const uint8_t *s, uint64_t len, uint64_t seed) {
  return hashLen16(cityHash64(s, len) - k2, seed, k3);
}

}  // namespace bytesteady
