#include <boost/dynamic_bitset.hpp>
#include <iostream>

#ifndef BITRLE_HPP
#define BITRLE_HPP

namespace sparsegray
{

template <typename TRun, typename TSize> std::vector<char> bitrle_encode(const boost::dynamic_bitset<> &input)
{
    std::vector<char> encoded;
    encoded.reserve(input.size() + 1);
    std::cout << input.size() + 1 << std::endl;

    auto append_bits = [&](uint_fast16_t value, std::size_t bits) {
        for (std::size_t i = 0; i < bits; i += 8)
        {
            encoded.push_back(static_cast<char>((value >> i) & 0xFF));
        }
    };

    // Append the input size at the beginning
    append_bits(static_cast<TSize>(input.size()), sizeof(TSize) * CHAR_BIT);

    TRun run_length = 0;
    bool current_bit = input[0];
    for (std::size_t i = 0; i < input.size(); ++i)
    {
        if (input[i] == current_bit)
        {
            ++run_length;
        }
        else
        {
            append_bits(static_cast<TRun>(run_length |
                                          (static_cast<uint_fast16_t>(current_bit) << (sizeof(TRun) * CHAR_BIT - 1))),
                        sizeof(TRun) * CHAR_BIT);
            current_bit = input[i];
            run_length = 1;
        }

        if (run_length == (static_cast<TRun>(1) << (sizeof(TRun) * CHAR_BIT - 1)) || i == input.size() - 1)
        {
            append_bits(static_cast<TRun>(run_length |
                                          (static_cast<uint_fast16_t>(current_bit) << (sizeof(TRun) * CHAR_BIT - 1))),
                        sizeof(TRun) * CHAR_BIT);
            run_length = 0;
        }
    }

    return encoded;
}

template <typename TRun, typename TSize> boost::dynamic_bitset<> bitrle_decode(const std::vector<char> &encoded)
{
    std::size_t pos = 0;

    auto read_bits = [&](std::size_t bits) -> uint_fast16_t {
        uint_fast16_t value = 0;
        for (std::size_t i = 0; i < bits; ++i)
        {
            value |= static_cast<uint_fast16_t>((encoded[pos / 8] >> (pos % 8)) & 1) << i;
            ++pos;
        }
        return value;
    };

    // Read the input size from the beginning
    TSize original_size = static_cast<TSize>(read_bits(sizeof(TSize) * CHAR_BIT));
    boost::dynamic_bitset<> decoded(original_size);

    std::size_t decoded_pos = 0;
    while (pos < encoded.size() * CHAR_BIT)
    {
        TRun run = static_cast<TRun>(read_bits(sizeof(TRun) * CHAR_BIT));

        bool current_bit = run >> (sizeof(TRun) * CHAR_BIT - 1);
        std::size_t run_length = run & (~(static_cast<TRun>(1) << (sizeof(TRun) * CHAR_BIT - 1)));

        for (std::size_t i = 0; i < run_length; ++i)
        {
            decoded[decoded_pos++] = current_bit;
        }
    }

    return decoded;
}

} // namespace sparsegray

#endif // BITRLE_HPP