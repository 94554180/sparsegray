#include <array>
#include <bitset>
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <vector>

#ifndef GRAYSCALE_MAT_HPP
#define GRAYSCALE_MAT_HPP

namespace sparsegray
{

struct GrayscaleMatrix
{
    GrayscaleMatrix(size_t rows, size_t cols) : rows(rows), cols(cols), count(rows * cols), data(rows * cols)
    {
    }

    size_t rows;
    size_t cols;
    size_t count;
    boost::dynamic_bitset<> data;

    bool operator==(const GrayscaleMatrix &other) const
    {
        return data == other.data && rows == other.rows && cols == other.cols;
    }

    bool operator!=(const GrayscaleMatrix &other) const
    {
        return !(*this == other);
    }

    inline void set_bit(size_t row, size_t col, bool value)
    {
        assert(row < rows && col < cols);
        size_t bit_pos = row * cols + col;
        data.set(bit_pos, value);
    }

    inline bool get_bit(size_t row, size_t col) const
    {
        assert(row < rows && col < cols);
        size_t bit_pos = row * cols + col;
        return data.test(bit_pos);
    }

    void downsample(size_t h_scale, size_t v_scale)
    {
        assert(h_scale > 0 && v_scale > 0);

        size_t new_rows = rows / v_scale;
        size_t new_cols = cols / h_scale;

        for (size_t r = 0; r < new_rows; ++r)
        {
            for (size_t c = 0; c < new_cols; ++c)
            {
                size_t src_row = r * v_scale;
                size_t src_col = c * h_scale;
                bool value = get_bit(src_row, src_col);
                set_bit(r, c, value);
            }
        }

        // Update matrix dimensions and data storage
        rows = new_rows;
        cols = new_cols;
        count = new_rows * new_cols;
        data.resize(count);
    }

    auto operator^(const GrayscaleMatrix &other) const
    {
        GrayscaleMatrix result{rows, cols};
        result.data = data ^ other.data;
        return result;
    }

    auto &operator^=(const GrayscaleMatrix &other)
    {
        assert(rows == other.rows && cols == other.cols);
        for (size_t i = 0; i < count; ++i)
        {
            data[i] ^= other.data[i];
        }
        return *this;
    }

    void copy_data_to(void *dest) const
    {
        // CAUTION! THIS PROBABLY DOESN'T WORK, AND BECAUSE IT LACKS A DESTINATION SIZE, IT'S REALLY UNSAFE!
        std::memcpy(dest, &data, data.size() / CHAR_BIT);
    }
};

struct SubGrayscaleMatrixView
{
    SubGrayscaleMatrixView(GrayscaleMatrix &matrix, size_t start_row, size_t start_col, size_t sub_rows,
                           size_t sub_cols)
        : matrix(matrix), start_row(start_row), start_col(start_col), sub_rows(sub_rows), sub_cols(sub_cols)
    {
    }

    GrayscaleMatrix &matrix;
    size_t start_row, start_col;
    size_t sub_rows, sub_cols;

    void set_bit(size_t row, size_t col, bool value)
    {
        assert(row < sub_rows && col < sub_cols);
        matrix.set_bit(start_row + row, start_col + col, value);
    }

    bool get_bit(size_t row, size_t col) const
    {
        assert(row < sub_rows && col < sub_cols);
        return matrix.get_bit(start_row + row, start_col + col);
    }

    void copy_data_to(char *dest) const
    {
        assert(dest != nullptr);
        size_t dest_bit_pos = 0;

        for (size_t row = 0; row < sub_rows; ++row)
        {
            for (size_t col = 0; col < sub_cols; ++col)
            {
                bool value = get_bit(row, col);

                size_t item_pos = dest_bit_pos / CHAR_BIT;
                size_t item_bit_pos = dest_bit_pos % CHAR_BIT;

                if (value)
                {
                    dest[item_pos] |= (1 << item_bit_pos);
                }
                else
                {
                    dest[item_pos] &= ~(1 << item_bit_pos);
                }

                ++dest_bit_pos;
            }
        }
    }
};

} // namespace sparsegray

#endif // GRAYSCALE_MAT_HPP