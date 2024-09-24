#ifndef BITS_CODER_INCLUDE_BITS_CODER_DEFINES_H_
#define BITS_CODER_INCLUDE_BITS_CODER_DEFINES_H_

#include <cstdint>
#include <cstddef>
#include <cstring>

#define BITS_CODER_BEGIN_NAMESPACE namespace bits_coder {
#define BITS_CODER_END_NAMESPACE }

BITS_CODER_BEGIN_NAMESPACE

enum class ByteOrders
{
    kBe = 0,
    kLe,
};

constexpr uint8_t kBitMasks[] =
{
    0b11111111,
    0b01111111,
    0b00111111,
    0b00011111,
    0b00001111,
    0b00000111,
    0b00000011,
    0b00000001,
};

constexpr uint8_t kBitNegMasks[] =
{
    0b00000000,
    0b10000000,
    0b11000000,
    0b11100000,
    0b11110000,
    0b11111000,
    0b11111100,
    0b11111110,
    0b11111111,
};

constexpr uint8_t kBitPoints[] =
{
    0b10000000,
    0b01000000,
    0b00100000,
    0b00010000,
    0b00001000,
    0b00000100,
    0b00000010,
    0b00000001,
};

struct SavePoint
{
    uint8_t *ptr = nullptr;
    size_t bit_pos = 0;
};

BITS_CODER_END_NAMESPACE

#endif // !BITS_CODER_INCLUDE_BITS_CODER_DEFINES_H_
