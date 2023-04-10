#include "../bitrle.hpp"
#include "../grayscale_mat.hpp"
#include "../sgf_codec.hpp"
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

void print_matrix(const sparsegray::GrayscaleMatrix &matrix)
{
    for (size_t row = 0; row < matrix.rows; row++)
    {
        for (size_t col = 0; col < matrix.cols; col++)
        {
            bool value = matrix.get_bit(row, col);
            std::cout << (value ? "1" : "0") << " ";
        }
        std::cout << std::endl;
    }
}

void print_sub_matrix(const sparsegray::SubGrayscaleMatrixView &matrix_view)
{
    for (size_t row = 0; row < matrix_view.sub_rows; ++row)
    {
        for (size_t col = 0; col < matrix_view.sub_cols; ++col)
        {
            bool value = matrix_view.get_bit(row, col);
            std::cout << (value ? "1" : "0") << " ";
        }
        std::cout << "\n";
    }
}

void print_char_vector_bits(const std::vector<char> &vec)
{
    for (size_t i = 0; i < vec.size(); ++i)
    {
        std::cout << std::bitset<8>(vec[i]);
    }
    std::cout << "\n";
}

template <auto m, auto n> void random_matrix(sparsegray::GrayscaleMatrix &matrix)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<unsigned long long> distrib(0, std::numeric_limits<unsigned long long>::max());

    const size_t num_bits = matrix.data.size();
    const size_t num_ull = (num_bits + 8 * sizeof(unsigned long long) - 1) / (8 * sizeof(unsigned long long));
    for (size_t i = 0; i < num_ull; ++i)
    {
        unsigned long long ull_value = distrib(gen);
        for (size_t bit = 0; bit < 8 * sizeof(unsigned long long); ++bit)
        {
            size_t bit_pos = i * 8 * sizeof(unsigned long long) + bit;
            if (bit_pos < num_bits)
            {
                bool value = (ull_value >> bit) & 1;
                matrix.data.set(bit_pos, value);
            }
        }
    }
}

void GrayscaleMatrix_cmp_test()
{
    cout << "GrayscaleMatrix == test" << endl;

    sparsegray::GrayscaleMatrix matrix1(64, 128);
    sparsegray::GrayscaleMatrix matrix2(64, 128);

    cout << "M_1 = M_2? " << (matrix1 == matrix2) << endl;
    cout << endl << endl;
}

void GrayscalMatrix_downsample_test()
{
    cout << "GrayscaleMatrix in-situ downsampling test" << endl;

    sparsegray::GrayscaleMatrix matrix(64, 128);
    const std::bitset<768> matrix_iv(
        "11111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110"
        "10101010100100010011111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100"
        "01001111111111111010101010100100010011111111111110101010101001000100111111111111101010101010010001001111111111"
        "11101010101010010001001111111111111010101010100100010011111111111110101010101001000100111111111111101010101010"
        "01000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010101001000100111111"
        "11111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010"
        "101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010101001000100");
    for (size_t i = 0; i < matrix_iv.size(); ++i)
    {
        matrix.data[i] = matrix_iv[i];
    }

    cout << "Input matrix size: " << matrix.data.size() << " bits" << endl;
    std::cout << "Input matrix:" << std::endl;
    print_matrix(matrix);

    matrix.downsample(2, 2);

    cout << "Output matrix size: " << matrix.data.size() << " bits" << endl;
    std::cout << "Output matrix:" << std::endl;
    print_matrix(matrix);
    cout << endl << endl;
}

void SubGrayscaleMatrixView_constructor_test()
{
    cout << "GrayscaleMatrix constructor test" << endl;

    sparsegray::GrayscaleMatrix matrix(8, 8);
    matrix.set_bit(1, 1, true);
    matrix.set_bit(3, 3, true);
    matrix.set_bit(5, 5, true);
    matrix.set_bit(7, 7, true);
    print_matrix(matrix);

    sparsegray::SubGrayscaleMatrixView sub_matrix(matrix, 2, 2, 4, 4);
    print_sub_matrix(sub_matrix);

    assert(sub_matrix.get_bit(0, 0) == false);
    assert(sub_matrix.get_bit(1, 1) == true);
    assert(sub_matrix.get_bit(2, 2) == false);
    assert(sub_matrix.get_bit(3, 3) == true);

    std::cout << "SubGrayscaleMatrixView constructor test passed!" << std::endl;
    cout << endl << endl;
}

void GrayscaleMatrix_xor_eq_view_test()
{
    cout << "GrayscaleMatrix ^= test" << endl;

    sparsegray::GrayscaleMatrix matrix1(2, 2);
    matrix1.set_bit(0, 0, true);
    matrix1.set_bit(0, 1, false);
    matrix1.set_bit(1, 0, true);
    matrix1.set_bit(1, 1, false);

    sparsegray::GrayscaleMatrix matrix2(2, 2);
    matrix2.set_bit(0, 0, true);
    matrix2.set_bit(0, 1, true);
    matrix2.set_bit(1, 0, false);
    matrix2.set_bit(1, 1, true);

    sparsegray::GrayscaleMatrix expected_result(2, 2);
    expected_result.set_bit(0, 0, false);
    expected_result.set_bit(0, 1, true);
    expected_result.set_bit(1, 0, true);
    expected_result.set_bit(1, 1, true);

    print_matrix(matrix1);
    print_matrix(matrix2);
    print_matrix(expected_result);

    matrix1 ^= matrix2;

    if (matrix1 == expected_result)
    {
        std::cout << "GrayscaleMatrix ^= test passed." << std::endl;
    }
    else
    {
        std::cout << "GrayscaleMatrix ^= test failed." << std::endl;
    }
    cout << endl << endl;
}

void bitrle_encode_test()
{
    std::cout << "bitrle_encode test" << std::endl;

    sparsegray::GrayscaleMatrix matrix(64, 128);
    const std::bitset<1536> matrix_iv(
        "11111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110"
        "10101010100100010011111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100"
        "01001111111111111010101010100100010011111111111110101010101001000100111111111111101010101010010001001111111111"
        "11101010101010010001001111111111111010101010100100010011111111111110101010101001000100111111111111101010101010"
        "01000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010101001000100111111"
        "11111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010"
        "10100100010011111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011"
        "111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110"
        "10101010100100010011111111111110101010101001000100111111111111101010101010010001001111111111111010101010100100"
        "01001111111111111010101010100100010011111111111110101010101001000100111111111111101010101010010001001111111111"
        "11101010101010010001001111111111111010101010100100010011111111111110101010101001000100111111111111101010101010"
        "01000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010101001000100111111"
        "11111110101010101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010"
        "101001000100111111111111101010101010010001001111111111111010101010100100010011111111111110101010101001000100");
    for (size_t i = 0; i < matrix_iv.size(); ++i)
    {
        matrix.data[i] = matrix_iv[i];
    }
    auto input = matrix.data;

    auto encoded = sparsegray::bitrle_encode<uint8_t, uint16_t>(input);
    auto decoded = sparsegray::bitrle_decode<uint8_t, uint16_t>(encoded);

    std::cout << "Original size: " << input.size() << std::endl;
    std::cout << "Original: " << input << std::endl;
    std::cout << "Encoded size: " << encoded.size() * 8 << std::endl;
    std::cout << "Encoded: ";
    print_char_vector_bits(encoded);
    std::cout << "Decoded: " << decoded << std::endl;
    cout << endl << endl;
}

void sgf_codec_test()
{
    std::cout << "sgf_codec test" << std::endl;

    constexpr size_t rows = 10;
    constexpr size_t cols = 10;

    // Create two grayscale matrices
    sparsegray::GrayscaleMatrix frame1(rows, cols);
    sparsegray::GrayscaleMatrix frame2(rows, cols);

    for (size_t row = 0; row < rows; ++row)
    {
        for (size_t col = 0; col < cols; ++col)
        {
            frame1.set_bit(row, col, (row + col) % 2);
            frame2.set_bit(row, col, (row + col + 1) % 2);
        }
    }

    // Initialize encoder and decoder
    sparsegray::SgfEncoder<uint8_t, uint16_t> encoder(rows, cols);
    sparsegray::SgfDecoder<uint8_t, uint16_t> decoder(rows, cols);

    // Set initial frames
    encoder.set_initial_frame(frame1);
    decoder.set_initial_frame(frame1);

    // Encode and decode frame2
    std::vector<char> encoded_frame2 = encoder.encode(frame2);
    sparsegray::GrayscaleMatrix decoded_frame2 = decoder.decode(encoded_frame2);

    cout << "encoded_frame2 size in bits: " << encoded_frame2.size() * 8 << endl;
    cout << "encoded_frame2: " << endl;
    print_char_vector_bits(encoded_frame2);
    cout << "frame1: " << endl;
    print_matrix(frame1);
    cout << "frame2: " << endl;
    print_matrix(frame2);
    cout << "decoded_frame2: " << endl;
    print_matrix(decoded_frame2);

    // Verify that decoded_frame2 matches frame2
    assert(decoded_frame2 == frame2);

    std::cout << "Test passed!" << std::endl;
    cout << endl << endl;
}

int main()
{
    GrayscaleMatrix_cmp_test();                // OK
    GrayscalMatrix_downsample_test();          // OK
    SubGrayscaleMatrixView_constructor_test(); // OK
    GrayscaleMatrix_xor_eq_view_test();        // OK
    SubGrayscaleMatrixView_constructor_test(); //
    bitrle_encode_test();                      //
    sgf_codec_test();                          //

    return 0;
}