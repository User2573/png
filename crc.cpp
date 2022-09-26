#include <iostream>
#include <iomanip>
#include <cstddef>
#include <array>

/* Table of CRCs of all 8-bit messages. */
uint32_t crc_table[256];

/* Flag: has the table been computed? Initially false. */
bool crc_table_computed = false;

/* Make the table for a fast CRC. */
void make_crc_table()
{
    uint32_t c;
    int n, k;

    for (n = 0; n < 256; n++) {
        c = (uint32_t)n;
        for (k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    crc_table_computed = true;
}

/* Update a running CRC with the bytes buf[0..len-1]--the CRC
   should be initialized to all 1's, and the transmitted value
   is the 1's complement of the final running CRC (see the
   crc() routine below)). */

uint32_t update_crc(uint32_t crc, std::byte* buf,
    int len)
{
    uint32_t c = crc;
    int n;

    if (!crc_table_computed)
        make_crc_table();
    for (n = 0; n < len; n++) {
        c = crc_table[(c ^ std::to_integer<uint8_t>(buf[n])) & 0xff] ^ (c >> 8);
    }
    return c;
}

/* Return the CRC of the bytes buf[0..len-1]. */
uint32_t crc(std::byte* buf, int len)
{
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

int main()
{
    uint8_t input[]{
        0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0xe0,
        0x00, 0x00, 0x01, 0x40,
        0x08,
        0x02,
        0x00,
        0x00,
        0x00
    };
    uint32_t crcVal = crc(reinterpret_cast<std::byte*>(input), std::size(input));
    //uint8_t expected[]{ 0xeb, 0xaa, 0x38, 0x2c };

    std::cout << std::hex << crcVal << std::endl;




    std::cout << std::endl;


    static const auto printBytes = []<typename T>(T x)
    {
        std::byte* ptr = reinterpret_cast<std::byte*>(&x);

        std::cout << std::hex << std::left;
        for (size_t i = 0; i < sizeof(x); i++)
            std::cout << std::setw(2) << std::to_integer<unsigned short>(ptr[i]) << " ";
        std::cout << std::endl;
    };

    static const auto toEndian = []<typename T>(const T& x, std::endian desired = std::endian::big) -> T
    {
        if (std::endian::native == desired)
            return x;
        return std::byteswap(x);
    };

    uint32_t val1 = 0xDEADBEEF;
    printBytes(val1);

    uint32_t val2 = 123;
    printBytes(val2);

    printBytes(std::byteswap(val1));

    printBytes(crcVal);

    printBytes(toEndian(crcVal));
}
