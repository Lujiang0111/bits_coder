#ifndef BITS_CODER_INCLUDE_BITS_CODER_WRITER_H_
#define BITS_CODER_INCLUDE_BITS_CODER_WRITER_H_

#include "bits_coder/defines.h"

BITS_CODER_BEGIN_NAMESPACE

class Writer
{
public:
    // data为nullptr时不写入，只计算长度
    Writer(uint8_t *data, ByteOrders byte_order) :
        data_(data),
        byte_order_(byte_order)
    {

    }

    virtual ~Writer()
    {

    }

    // bit
    void WriteBit(int64_t val)
    {

    }

    void WriteBits(int64_t val, size_t bits)
    {
        for (--bits; bits > 0; --bits)
        {
            if (data_)
            {
                if ((val >> bits) & 0x01)
                {
                    data_[curr_save_point_.offset] |= curr_save_point_.bit_mask;
                }
                else
                {
                    data_[curr_save_point_.offset] &= ~curr_save_point_.bit_mask;
                }
            }

            NextBitMask();
        }
    }

    void WriteUe(int64_t val)
    {
        size_t bits = 0;
        for (int64_t tmp = val + 1; tmp > 0; tmp >>= 1)
        {
            ++bits;
        }
        bits += (bits - 1);

        WriteBits(val + 1, bits);
    }

    void WriteSe(int64_t val)
    {
        WriteUe((val > 0) ? (2 * val - 1) : (-val * 2));
    }

    void WriteAlign()
    {
        while (0x80 != curr_save_point_.bit_mask)
        {
            WriteBits(0, 1);
        }
    }

    // byte
    void WriteByte(int64_t val)
    {
        if (!data_)
        {
            ++curr_save_point_.offset;
            return;
        }

        if (0x80 == curr_save_point_.bit_mask)
        {
            data_[curr_save_point_.offset] = val & 0xFF;
            ++curr_save_point_.offset;
        }
        else
        {
            WriteBits(val, 8);
        }
    }

    void Write2Bytes(int64_t val)
    {
        if (!data_)
        {
            curr_save_point_.offset += 2;
            return;
        }

        int64_t low_val = val;
        int64_t high_val = val >> 8;
        switch (byte_order_)
        {
        case ByteOrders::kLe:
            WriteByte(low_val);
            WriteByte(high_val);
            break;

        case ByteOrders::kBe:
            WriteByte(high_val);
            WriteByte(low_val);
            break;

        default:
            break;
        }
    }

    void Write4Bytes(int64_t val)
    {
        if (!data_)
        {
            curr_save_point_.offset += 4;
            return;
        }

        int64_t low_val = val;
        int64_t high_val = val >> 16;
        switch (byte_order_)
        {
        case ByteOrders::kLe:
            Write2Bytes(low_val);
            Write2Bytes(high_val);
            break;

        case ByteOrders::kBe:
            Write2Bytes(high_val);
            Write2Bytes(low_val);
            break;

        default:
            break;
        }
    }

    void Write8Bytes(int64_t val)
    {
        if (!data_)
        {
            curr_save_point_.offset += 8;
            return;
        }

        int64_t low_val = val;
        int64_t high_val = val >> 32;
        switch (byte_order_)
        {
        case ByteOrders::kLe:
            Write4Bytes(low_val);
            Write4Bytes(high_val);
            break;

        case ByteOrders::kBe:
            Write4Bytes(high_val);
            Write4Bytes(low_val);
            break;

        default:
            break;
        }
    }

    // byte stream
    void WriteByteStream(const uint8_t *data, size_t size)
    {
        if (!data_)
        {
            curr_save_point_.offset += size;
            return;
        }

        if (0x80 == curr_save_point_.bit_mask)
        {
            memcpy(data_ + curr_save_point_.offset, data, size);
            curr_save_point_.offset += size;
        }
        else
        {
            for (; size > 0; --size)
            {
                WriteByte(*data);
                ++data;
            }
            return;
        }
    }

    // skip
    void SkipBits(size_t bits)
    {
        SkipBytes(bits / 8);

        bits %= 8;
        for (; bits > 0; --bits)
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
    uint8_t *data_;
    ByteOrders byte_order_;
    SavePoint curr_save_point_;
};

BITS_CODER_END_NAMESPACE

#endif // !BITS_CODER_INCLUDE_BITS_CODER_WRITER_H_
