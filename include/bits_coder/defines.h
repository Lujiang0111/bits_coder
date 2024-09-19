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
    kLe = 0,
    kBe,
};

struct SavePoint
{
    size_t offset = 0;
    uint8_t bit_mask = 0x80;
};

BITS_CODER_END_NAMESPACE

#endif // !BITS_CODER_INCLUDE_BITS_CODER_DEFINES_H_
