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
    GrayscaleMatrix(size_t cols, size_t rows) : cols(cols), rows(rows), count(rows * cols), data(rows * cols)
    {
    }

    size_t cols;
    size_t rows;
    size_t count;
    boost::dynamic_bitset<> data;

    bool operator==(const GrayscaleMatrix &other) const
    {
        return cols == other.cols && rows == other.rows && data == other.data;
    }

    bool operator!=(const GrayscaleMatrix &other) const
    {
        return !(*this == other);
    }

    inline void set_bit(size_t col, size_t row, bool value)
    {
        assert(col < cols && row < rows);
        size_t bit_pos = row * cols + col;
        data.set(bit_pos, value);
    }

    inline bool get_bit(size_t col, size_t row) const
    {
        assert(col < cols && row < rows);
        size_t bit_pos = row * cols + col;
        return data.test(bit_pos);
    }

    void downsample(size_t h_scale, size_t v_scale)
    {
        assert(h_scale > 0 && v_scale > 0);

        size_t new_cols = cols / h_scale;
        size_t new_rows = rows / v_scale;

        for (size_t row = 0; row < new_rows; ++row)
        {
            for (size_t col = 0; col < new_cols; ++col)
            {
                size_t src_col = col * h_scale;
                size_t src_row = row * v_scale;
                bool value = get_bit(src_col, src_row);
                set_bit(col, row, value);
            }
        }

        // Update matrix dimensions and data storage
        cols = new_cols;
        rows = new_rows;
        count = new_cols * new_rows;
        data.resize(count);
    }

    auto operator^(const GrayscaleMatrix &other) const
    {
        GrayscaleMatrix result{cols, rows};
        result.data = data ^ other.data;
        return result;
    }

    auto &operator^=(const GrayscaleMatrix &other)
    {
        assert(cols == other.cols && rows == other.rows);
        for (size_t i = 0; i < count; ++i)
        {
            data[i] ^= other.data[i];
        }
        return *this;
    }
};

struct SubGrayscaleMatrixView
{
    SubGrayscaleMatrixView(GrayscaleMatrix &matrix, size_t start_col, size_t start_row, size_t sub_cols,
                           size_t sub_rows)
        : matrix(matrix), start_col(start_col), start_row(start_row), sub_cols(sub_cols), sub_rows(sub_rows)
    {
    }

    GrayscaleMatrix &matrix;
    size_t start_col, start_row;
    size_t sub_cols, sub_rows;

    void set_bit(size_t row, size_t col, bool value)
    {
        assert(col < sub_cols && row < sub_rows);
        matrix.set_bit(start_col + col, value, start_row + row);
    }

    bool get_bit(size_t row, size_t col) const
    {
        assert(row < sub_rows && col < sub_cols);
        return matrix.get_bit(start_col + col, start_row + row);
    }

    void copy_data_to(char *dest) const
    {
        assert(dest != nullptr);
        size_t dest_bit_pos = 0;

        for (size_t row = 0; row < sub_rows; ++row)
        {
            for (size_t col = 0; col < sub_cols; ++col)
            {
                bool value = get_bit(col, row);

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