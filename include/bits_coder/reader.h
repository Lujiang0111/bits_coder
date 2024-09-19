#ifndef BITS_CODER_INCLUDE_BITS_CODER_READER_H_
#define BITS_CODER_INCLUDE_BITS_CODER_READER_H_

#include "bits_coder/defines.h"

BITS_CODER_BEGIN_NAMESPACE

class Reader
{
public:
    Reader(const uint8_t *data, ByteOrders byte_order) :
        data_(data),
        byte_order_(byte_order)
    {

    }

    virtual ~Reader()
    {

    }

    // bit
    int64_t ReadBits(size_t bits)
    {
        int64_t val = 0;
        for (; bits > 0; --bits)
        {
            val = (val << 1) | ((data_[curr_save_point_.offset] & curr_save_point_.bit_mask) ? 1 : 0);
            NextBitMask();
        }
        return val;
    }

    int64_t ReadUe()
    {
        int64_t leading_zero_bits = 0;
        while (0 == ReadBits(1))
        {
            ++leading_zero_bits;
        }
        return (1LL << leading_zero_bits) + ReadBits(leading_zero_bits) - 1;
    }

    int64_t ReadSe()
    {
        int64_t val = ReadUe();
        if (0 == val % 2)
        {
            val = -val / 2;
        }
        else
        {
            val = val / 2 + 1;
        }
        return val;
    }

    void ReadAlign()
    {
        if (0x80 != curr_save_point_.bit_mask)
        {
            ++curr_save_point_.offset;
            curr_save_point_.bit_mask = 0x80;
        }
    }

    // byte
    int64_t ReadByte()
    {
        int64_t val = 0;
        if (0x80 == curr_save_point_.bit_mask)
        {
            val = data_[curr_save_point_.offset];
            ++curr_save_point_.offset;
        }
        else
        {
            val = ReadBits(8);
        }

        return val;
    }

    int64_t Read2Bytes()
    {
        int64_t low_val = 0;
        int64_t high_val = 0;
        switch (byte_order_)
        {
        case ByteOrders::kLe:
            low_val = ReadByte();
            high_val = ReadByte();
            break;

        case ByteOrders::kBe:
            high_val = ReadByte();
            low_val = ReadByte();
            break;

        default:
            break;
        }

        return (high_val << 8) + low_val;
    }

    int64_t Read4Bytes()
    {
        int64_t low_val = 0;
        int64_t high_val = 0;
        switch (byte_order_)
        {
        case ByteOrders::kLe:
            low_val = Read2Bytes();
            high_val = Read2Bytes();
            break;

        case ByteOrders::kBe:
            high_val = Read2Bytes();
            low_val = Read2Bytes();
            break;

        default:
            break;
        }

        return (high_val << 16) + low_val;
    }

    int64_t Read8Bytes()
    {
        int64_t low_val = 0;
        int64_t high_val = 0;
        switch (byte_order_)
        {
        case ByteOrders::kLe:
            low_val = Read4Bytes();
            high_val = Read4Bytes();
            break;

        case ByteOrders::kBe:
            high_val = Read4Bytes();
            low_val = Read4Bytes();
            break;

        default:
            break;
        }

        return (high_val << 32) + low_val;
    }

    // byte stream
    void ReadByteStream(uint8_t *buf, size_t size)
    {
        if (0x80 == curr_save_point_.bit_mask)
        {
            memcpy(buf, data_ + curr_save_point_.offset, size);
            curr_save_point_.offset += size;
        }
        else
        {
            for (; size > 0; --size)
            {
                *buf++ = static_cast<uint8_t>(ReadByte());
            }
        }
    }

    // skip
    void SkipBits(size_t bits)
    {
        SkipBytes(bits / 8);

        bits %= 8;
        for (size_t i = 0; i < bits; ++i)
        {
            NextBitMask();
        }
    }

    void SkipBytes(size_t bytes)
    {
        curr_save_point_.offset += bytes;
    }

    // save/load
    const SavePoint &Save() const
    {
        return curr_save_point_;
    }

    void Load(const SavePoint &save_point)
    {
        curr_save_point_ = save_point;
    }

    size_t CurrSize() const
    {
        return (0x80 == curr_save_point_.bit_mask) ? (curr_save_point_.offset) : (curr_save_point_.offset + 1);
    }

private:
    void NextBitMask()
    {
        curr_save_point_.bit_mask >>= 1;
        if (0 == curr_save_point_.bit_mask)
        {
            ++curr_save_point_.offset;
            curr_save_point_.bit_mask = 0x80;
        }
    }

private:
    const uint8_t *data_;
    ByteOrders byte_order_;
    SavePoint curr_save_point_;
};

BITS_CODER_END_NAMESPACE

#endif // !BITS_CODER_INCLUDE_BITS_CODER_READER_H_
