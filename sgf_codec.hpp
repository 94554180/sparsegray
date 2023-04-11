#include "bitrle.hpp"
#include "grayscale_mat.hpp"
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <vector>

#ifndef SGF_CODEC_HPP
#define SGF_CODEC_HPP

namespace sparsegray
{

template <typename TRun, typename TSize> struct SgfEncoder
{
    GrayscaleMatrix latest_frame;

    SgfEncoder(size_t rows, size_t cols) : latest_frame(rows, cols)
    {
    }

    void set_initial_frame(const GrayscaleMatrix &frame)
    {
        assert(latest_frame.rows == frame.rows && latest_frame.cols == frame.cols);
        latest_frame = frame;
    }

    std::vector<char> encode(const GrayscaleMatrix &frame)
    {
        assert(latest_frame.rows == frame.rows && latest_frame.cols == frame.cols);
        GrayscaleMatrix delta_frame = latest_frame ^ frame;
        latest_frame = frame;
        return bitrle_encode<TRun, TSize>(delta_frame.data);
    }
};

template <typename TRun, typename TSize> struct SgfDecoder
{
    GrayscaleMatrix latest_frame;

    SgfDecoder(size_t rows, size_t cols) : latest_frame(rows, cols)
    {
    }

    void set_initial_frame(const GrayscaleMatrix &frame)
    {
        assert(latest_frame.rows == frame.rows && latest_frame.cols == frame.cols);
        latest_frame = frame;
    }

    GrayscaleMatrix decode(const std::vector<char> &encoded_data)
    {
        boost::dynamic_bitset<> decoded_data = bitrle_decode<TRun, TSize>(encoded_data);
        GrayscaleMatrix delta_frame(latest_frame.rows, latest_frame.cols);

        for (size_t i = 0; i < latest_frame.count; ++i)
        {
            bool xor_result = latest_frame.data[i] ^ decoded_data[i];
            delta_frame.data.set(i, xor_result);
            latest_frame.data.set(i, xor_result);
        }

        return delta_frame;
    }
};

} // namespace sparsegray

#endif // SGF_CODEC_HPP