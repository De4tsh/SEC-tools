#include <iostream>
#include <vector>
#include <assert.h>
using namespace std;

/* Base58编码表中的58个字符 */
static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

/* 解码映射表 */
static const int8_t mapBase58[256] = {
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
    -1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
    22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
    -1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
    47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
};
constexpr inline bool IsSpace(char c) noexcept {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

bool DecodeBase58(const char* psz, std::vector<unsigned char>& vch, int max_ret_len)
{
    // Skip leading spaces.
    while (*psz && IsSpace(*psz))
        psz++;
    // Skip and count leading '1's.
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') {
        zeroes++;
        if (zeroes > max_ret_len) return false;
        psz++;
    }
    // Allocate enough space in big-endian base256 representation.
    int size = strlen(psz) * 733 / 1000 + 1; // log(58) / log(256), rounded up.
    std::vector<unsigned char> b256(size);
    // Process the characters.
    static_assert(sizeof(mapBase58) / sizeof(mapBase58[0]) == 256, "mapBase58.size() should be 256"); // guarantee not out of range
    while (*psz && !IsSpace(*psz)) {
        // Decode base58 character
        int carry = mapBase58[(uint8_t)*psz];
        if (carry == -1)  // Invalid b58 character
            return false;
        int i = 0;
        for (std::vector<unsigned char>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
            carry += 58 * (*it);
            *it = carry % 256;
            carry /= 256;
        }
        assert(carry == 0);
        length = i;
        if (length + zeroes > max_ret_len) return false;
        psz++;
    }
    // Skip trailing spaces.
    while (IsSpace(*psz))
        psz++;
    if (*psz != 0)
        return false;
    // Skip leading zeroes in b256.
    std::vector<unsigned char>::iterator it = b256.begin() + (size - length);
    // Copy result into output vector.
    vch.reserve(zeroes + (b256.end() - it));
    vch.assign(zeroes, 0x00);
    while (it != b256.end())
        vch.push_back(*(it++));
    return true;
}

/*
    pbegin 字符串的开始位置
    pend 字符串的结束位置
    std::string 返回一个编码后的字符串
*/

std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend)
{
    // 统计 0 的数量
    int zeroes = 0;
    int length = 0;
    while (pbegin != pend && *pbegin == 0)  // 若开头为0
    {
        pbegin++; // 头指针后移一位
        zeroes++; // 0的数量+1
    }
    // 计算编码后应该分配的空间大小
    /*
        (pend - pbegin) 得到字符串的大小 * 1.38 扩大到编码后的大小 + 1 向上取整
    */
    int size = (pend - pbegin) * 138 / 100 + 1; // log(256) / log(58), rounded up.
    std::vector<unsigned char> b58(size); // 输出使用vector容器存放
    // Process the bytes.
    while (pbegin != pend) {
        int carry = *pbegin;
        int i = 0;
        // Apply "b58 = b58 * 256 + ch".
        /*

            b58.rbegin() 其实迭代器指向vector的右开头 末尾
            b58.rend() 右结尾 也就是开头

            (carry != 0 || i < length) && (it != b58.rend());

                carry != 0 商 = 0表示这一次循环结束了
                i < length i用于作为下标遍历b58
        */
        for (auto it = b58.rbegin(); (carry != 0 || i < length) && (it != b58.rend()); it++, i++)
        {
            // 辗转相除法
            carry += 256 * (*it);  // *256相当于左移8位
            *it = carry % 58; // %58后得到余数
            carry /= 58;
        }

        assert(carry == 0);
        length = i; // 当前一个原数据char转换成了几个Base58编码
        pbegin++;
    }
    // Skip leading zeroes in base58 result.
    std::vector<unsigned char>::iterator it = b58.begin() + (size - length);
    while (it != b58.end() && *it == 0)
        it++;
    // Translate the result into a string.
    std::string str;
    str.reserve(zeroes + (b58.end() - it));
    str.assign(zeroes, '1');
    while (it != b58.end())
        str += pszBase58[*(it++)];
    return str;
}


int main(int argc, char argv[])
{
    unsigned char data[] = "测试base58数据";
    int len = sizeof(data);
    cout << data << endl;
    std::string re = EncodeBase58(data, data + len);
    cout << re << endl;
    std::vector<unsigned char> vsh;
    DecodeBase58(re.data(), vsh, 4096);
    cout << vsh.data() << endl;
    return 0;
}