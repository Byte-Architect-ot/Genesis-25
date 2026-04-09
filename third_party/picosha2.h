#ifndef PICOSHA2_H
#define PICOSHA2_H

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdint>

namespace picosha2 {

namespace detail {

inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}

inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}

inline uint32_t big_sigma0(uint32_t x) {
    return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
}

inline uint32_t big_sigma1(uint32_t x) {
    return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
}

inline uint32_t small_sigma0(uint32_t x) {
    return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
}

inline uint32_t small_sigma1(uint32_t x) {
    return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
}

inline void append_u64_be(std::vector<uint8_t>& data, uint64_t v) {
    data.push_back((uint8_t)((v >> 56) & 0xFF));
    data.push_back((uint8_t)((v >> 48) & 0xFF));
    data.push_back((uint8_t)((v >> 40) & 0xFF));
    data.push_back((uint8_t)((v >> 32) & 0xFF));
    data.push_back((uint8_t)((v >> 24) & 0xFF));
    data.push_back((uint8_t)((v >> 16) & 0xFF));
    data.push_back((uint8_t)((v >> 8) & 0xFF));
    data.push_back((uint8_t)(v & 0xFF));
}

inline uint32_t read_u32_be(const uint8_t* p) {
    return (uint32_t(p[0]) << 24) |
           (uint32_t(p[1]) << 16) |
           (uint32_t(p[2]) << 8)  |
           (uint32_t(p[3]));
}

} // namespace detail

inline std::string hash256_hex_string(const std::string& input) {
    static const uint32_t k[64] = {
        0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
        0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
        0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
        0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
        0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
        0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
        0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
        0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
    };

    uint32_t h[8] = {
        0x6a09e667u,0xbb67ae85u,0x3c6ef372u,0xa54ff53au,
        0x510e527fu,0x9b05688cu,0x1f83d9abu,0x5be0cd19u
    };

    std::vector<uint8_t> msg;
    msg.reserve(input.size() + 72);
    for (unsigned char c : input) msg.push_back((uint8_t)c);

    uint64_t bitLen = (uint64_t)msg.size() * 8ULL;

    msg.push_back(0x80);

    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    detail::append_u64_be(msg, bitLen);

    for (size_t offset = 0; offset < msg.size(); offset += 64) {
        uint32_t w[64];

        for (int i = 0; i < 16; i++) {
            w[i] = detail::read_u32_be(&msg[offset + i * 4]);
        }
        for (int i = 16; i < 64; i++) {
            w[i] = detail::small_sigma1(w[i - 2]) + w[i - 7] + detail::small_sigma0(w[i - 15]) + w[i - 16];
        }

        uint32_t a = h[0];
        uint32_t b = h[1];
        uint32_t c = h[2];
        uint32_t d = h[3];
        uint32_t e = h[4];
        uint32_t f = h[5];
        uint32_t g = h[6];
        uint32_t hh = h[7];

        for (int i = 0; i < 64; i++) {
            uint32_t t1 = hh + detail::big_sigma1(e) + detail::ch(e, f, g) + k[i] + w[i];
            uint32_t t2 = detail::big_sigma0(a) + detail::maj(a, b, c);
            hh = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
        h[4] += e;
        h[5] += f;
        h[6] += g;
        h[7] += hh;
    }

    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (int i = 0; i < 8; i++) {
        out << std::setw(8) << h[i];
    }
    return out.str();
}

} // namespace picosha2

#endif