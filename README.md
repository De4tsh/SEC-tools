# OpenSSL开发环境准备

## OpenSSL3.0介绍

OpenSSL是一个用于`TLS/SSL`协议的工具包，也是一个通用密码库

OpenSSL 3.0 Alpha2 Release支持 国密 SM2 SM3 SM4

### 

# Base64编解码

## Base64概述和应用场景

### 概述

二进制转字符串

### 应用场景

- 邮件编码（`base64`）

  附件也会经过base64编码

- XML或者是Json存储二进制内容

- 网页传递数据

- 数据库中以文本形式存放二进制数据

  数据库中一般存二进制，但二进制的读取较为麻烦，base64后会方便很多，但不能存储过大的数据转换开销过大

- **可打印的比特币钱包地址 `base58Check`（hash校验）**

  将二进制的数据转成通过ASCII这样可打印的字符

- 比特币地址 `bech32`（base32）

## Base16

Base的含义是进制，此处所谓的**Base16编码就是将2进制的数据转成16进制进行显示**

其中16进制数对应的字符串中的字符的ASCII码对应的为：

- 1 ~ 9 对应 ASCII码 49 ~ 57
- A ~ F 对应 ASCII码 65 ~ 70

由于Base16中需要将对应的二进制数据的值转为字符形式，Base 16中用4位表示一个字符，而正常一个字节应该是8位，所以我们需要对一个字节进行拆分，拆分为两个4位，而这两个4位由于需要被转化为字符形式，所以又要拓展到8位，就占用的空间相当于原来占用空间的一倍

Base16.cpp

```C++
#include <iostream>
using namespace std;

static const char BASE16_ENC_TAB[] = "0123456789ABCDEF";
static const char BASE16_DEC_TAB[128] = {
    
    -1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
    -1,-1,-1,-1,-1,-1,-1, 0, 1, 2,
     3, 4, 5, 6, 7, 8, 9,-1,-1,-1,	// 51 - 60
    -1,-1,-1,-1,10,11,12,13,14,15,	// 61 - 70 A ~ F
    
};  
    
    
}

// 对于二进制数据加上unsigned修饰
int fnBase16Encode(const unsigned char *in,int size,char *out)
{
    for (int i = 0; i < size; i++)
    {
        // 一个字节取出高四位 第四位
        char h = in[i] >> 4; // 1000 0001 >> 4 = 0000 1000
        char l = in[i] & b00001111; // 或与上 0x0F
        
        out[i * 2] = BASE16_ENC_TABLE[h]; // 0 ~ 15根据下标映射到对应字符
        out[1 + i * 2] = BASE16_ENC_TABLE[l];
    }   
    // base16 转码后空间扩大一倍
    // 4位转成一个8位字符
    return size << 1;
}

int Base16Decode(const string &in,usigned char* out)
{
    // 将两个字符拼接为一个字节
    for ( int i = 0; i < in.size(); i += 2 )
    {
        unsigned char ch = in[i]; // 高位转换的字符
        unsigned char cl = in[i + 1]; // 低位转换的字符
        
        unsigned char h = BASE16_DEC_TAB[ch]; // 转换成原来的值
        unsigned char l = BASE16_DEC_TAB[cl]；
            
        // 两个 4 位拼成一个字节8位
        // 若 h = 1000 l = 0001 则 h << 4 = 10000000 | l = 10000001
        out[i / 2] = h << 4 | l; 
    }
    
    
    return in.size() / 2
}



int main(int argc,char **argv)
{
    const unsigned char data[] = "测试Base16";
    int len = sizeof(data);
    char out1[1024] = {0};
    unsigned char* out2[1024] = {0};
    
    cout << data << endl;
    int re = Base16Encode(data,len,out1);
    cout << re << ":" << out1 << endl;
    re = Base16Decode(out1,out2);
    cout << re << ":" << out2;
    return 0;
}
```

## Base64（64进制）

`Base64`编码要求把**3个8位字节**（`3 * 8 = 24`）转化为**4个6位（二进制的6位能表示的最大值就是2 ^ 6 - 1 = 63,类比于Base16，此处相当于要用6位表示一个字符，3个字节转为4个字符）的字节**（`4 * 6 = 24`），之后在6位的前面补两个0，形成8位一个字节的形式。

如果剩下的字节不足3个8位字节，则用0填充，但直接补0是有问题的，因为我们原先的对应关系也有0，若是直接补0，恢复的时候会无法区分是原数据还是填充的，所以我们用 `=` 表示此处的数据是后填充的，转回的时候直接去掉 `=` 即可，因此编码后输出的文本末尾可能出现一个或两个 `=`

> 为什么要将三个8位转为四个6位，不同于Base16的四位直接将一个8位一分为二，6位无法分一个8位，所以需要和8取一个最大公约数24

### base64索引表

![49256676882d6328e36ae5c77ca708c7](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210021059667.png)

#### "NEW"的Base64编码

#### ![679c65bb8521c0a47206357436d7e19e](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210021101784.jpg)

#### 若只有一个字节

![e2363652dcdc65af5eea80f9aa9b7f32](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210021103059.jpg)

## OpenSSL的BIO接口

BIO包含了多种接口，用于控制在 `BIO_METHOD` 中的不同实现函数，包括6种 `fileter` 型和8种`source/sink `型

- `BIO_new` 创建一个BIO对象
- **数据源**：`source/sink` 类型的BIO是数据源 `BIO_new(BIO_s_mem())` **内存的数据源对象(`_s_` 是 source 的意思)**
  - 此处也可以从文件/socket读取
- **过滤**：`filter BIO`就是把数据从一个BIO转换到另外一个BIO或应用接口
  - `BIO_new(BIO_f_base64())` **创建一个base64过滤器用于处理数据源的数据（`_f_` 是filter的意思）**
- **BIO链**：一个BIO链通常包括一个`source BIO`和一个或多个`filter BIO`
  - `BIO_push(b64_bio,mem_bio);` **负责将数据源的数据进行多种处理（比如说先base64再MD5等等）**
- **写编码，读编码** `BIO_write BIO_read_ex`

**数据经过数据源通过过滤器进行编解码,再输出**

```C++
#include <iostream>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <time.h>
using namespace std;
#define __BUF_SIZE__	1024


// 编码
int Base64Encode(const unsigned char* in, int len, char* out_base64)
{

	if (!in || len <= 0 || !out_base64)
	{
		return 0;
	}

	// 内存元 source
	auto mem_bio = BIO_new(BIO_s_mem());
	if (!mem_bio)
	{
		return 0;
	}

	// Base64过滤器
	auto b64_bio = BIO_new(BIO_f_base64());
	if (!b64_bio)
	{
		BIO_free(mem_bio);
		return 0;
	}

	// 形成BIO链
	// b64 - mem
	BIO_push(b64_bio, mem_bio);

	// 超过64字节不添加换行符（\n），编码的数据在一行中
	BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);

	// 写入到b64 fileter进行编码，结果会传递到链表的下一个节点
	// 到mem读取结果（从链表头部读取 —— 链表头部代表整个链表）
	// BIO_write表示编码
	// 编码数据每64字节（不确定）会加 \n 换行符，可能对解码会有影响
	int re = BIO_write(b64_bio, in, len);
	if (re <= 0)
	{
		BIO_free_all(b64_bio);
		// 清空整个链表节点
		return 0;
	}

	// 刷新缓冲区,写入链表的mem
	BIO_flush(b64_bio);


	int outsize = 0;
	// 从链表源内存读取
	BUF_MEM* p_data = 0;
	BIO_get_mem_ptr(b64_bio, &p_data);
	if (p_data)
	{
		memcpy(out_base64, p_data->data, p_data->length);
		outsize = p_data->length;
	}

	BIO_free_all(b64_bio);
	return outsize;
}

// 解码 4字节转3字节
int Base64Decode(const char* in, int len, unsigned char* out_data)
{
	if (!in || len < 0 || !out_data)
	{
		return 0;
	}

	// 内存源（编码后的）
	// 解码
	auto mem_bio = BIO_new_mem_buf(in, len);
	if (!mem_bio)
	{
		return 0;
	}

	// base64过滤器
	auto b64_bio = BIO_new(BIO_f_base64());
	if (!b64_bio)
	{
		BIO_free(mem_bio);
		return 0;
	}

	// 形成BIO链
	BIO_push(b64_bio, mem_bio);


	// 默认读取换行符结束，若不包含换行符默认会出错，除非也包含以下语句
	BIO_set_flags(b64_bio, BIO_FLAGS_BASE64_NO_NL);
	// 此时如果编码中有 \n 会失败

	// 读取解码
	// 通过读写判断是解码还编码 读就位解码
	// 解码后的大小无法推算，需要根据密文推算
	size_t size = 0;
	BIO_read_ex(b64_bio, out_data,len ,&size); // 通过size返回解码后的大小
	BIO_free_all(b64_bio);
	return size;
}



int main()
{


	unsigned char data[] = "true 最终答案在 [left,mid] 区间中 right = mid";
	char outdata[__BUF_SIZE__] = {0};
	unsigned char outdata2[__BUF_SIZE__] = { 0 };
	int len = sizeof(data);
	size_t re;

	int nLength = Base64Encode(data, len, outdata);

	if (nLength > 0)
	{
		outdata[nLength] = '\0';
	}

	cout << "编码后的数据为：";
	cout << outdata << endl;

	re = Base64Decode(outdata, nLength, outdata2);

	cout << "解码了：" << re << "个字节，原文为：" << outdata2 << endl;

	return 0;


}
```

## Base58

> Base64的特点导致其输出的是一个字符串，但在比特币钱包地址的应用中，钱包地址要打印出来，给人看的话 0 o O l L 1区分困难，+ / 在URL地址传递中也会出现问题

`Base58`的编码集**在 `Base64` 字符集的基础上去掉了比较容易混淆的字符**，Base58不包含：

- O（保留了 o）
- 0
- l
- L
- +
- /

![292888-20191231091331205-295672581](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210031037819.png)

由于Base58无法像Base16与Base64那样快速的转换原字节与编码后字节的对应关系，而要依托于特定的算法，**所以Base58一般不用于数据的编码，而是用于一些地址栏传递的信息**

> OpenSSL等库并没有支持Base58

### Base58的转换方法

#### **辗转相除法**

也就是字符 `1` 代表 0，字符 `2 `代表 1，字符 `3` 代表 2....字符`z`代表 57，然后回一下辗转相除法

比如要将 1234 转为 58 进制：

- 1234 / 58 = 21 ... 16 查表得字符 `H`
- 21 / 58 = 0 ... 21 查表得字符 `N`

所以 **`1234` Base58编码后的结果位 `NH`** （先计算出得是低位）

若待转换得数前有0，则直接附加编码后的1来代表，有几个0附加几个1（编码表中1代表0）

### Base58输出字节数

对于我们最终要输出的数据，由于是char型的，所以是 8bit
$$
\log_2256
$$
那么最终输出数据的长度，
$$
Length2 = ( \log_2256 \times 字符数量)bits
$$
而由于这些数据时使用`Base58`编码后的，也就是说输出数据的一个char 8bit对应的一个原数据中的 
$$
\log_258位
$$
所以原数据的长度为：
$$
Length1 =  (\log_258 \times 字符数量)bits
$$
所以输出**数据的长度Length2相较于原数据扩大了**：
$$
\log_2256 \div \log_258 = 1.38倍
$$
1.38是约等于值

### 代码

> 比特币中Base58的实现代码

```C++
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

// noexcept 函数不抛出异常
constexpr inline bool IsSpace(char c) noexcept {
    return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';
}

bool DecodeBase58(const char* psz, std::vector<unsigned char>& vch, int max_ret_len)
{
    // Skip leading spaces. 去掉开头的非字符（IsSpace中的符号）部分
    while (*psz && IsSpace(*psz))
        psz++;
    // Skip and count leading '1's.
    int zeroes = 0;
    int length = 0;
    while (*psz == '1') { // 统计0的个数（编码后的1）
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
        int carry = mapBase58[(uint8_t)*psz]; // 将字符转为对应的数值
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
    while (IsSpace(*psz)) // 去空格
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
        for (auto it = b58.rbegin();(carry != 0 || i < length) && (it != b58.rend());it++, i++) 
        {
            carry += 256 * (*it);  // *256相当于左移8位
            *it = carry % 58; // %58后得到余数
            carry /= 58;
        }

        assert(carry == 0);
        length = i;
        pbegin++;
    }
    // Skip leading zeroes in base58 result.
    std::vector<unsigned char>::iterator it = b58.begin() + (size - length);
    while (it != b58.end() && *it == 0)
        it++;
    // Translate the result into a string.
    std::string str;
    str.reserve(zeroes + (b58.end() - it)); // 分配空间
    str.assign(zeroes, '1'); // 将0统一替换为映射后的1
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
```

