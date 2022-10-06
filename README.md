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
# 单向散列 消息指纹

## 单向Hash应用场景

- **文件完整性校验**
- **口令加密**
  - 口令 + 随机数salt 再散列
- **消息认证码（保证完整不被篡改）**
  - 发送者和接收者Hash（共享密钥 + 消息），防错误、篡改、伪装
  - `HMAC`
  - `SSL/TLS` 安全套接字通信 
- **伪随机数**
- **配合非对称加密做数字签名**
- **比特币工作量证明**

## 常用Hash算法

- `MD5`
- `SHA1`
- `SHA2`（`SHA-256` `SHA-384` `SHA-512`）（比特币）
- `SHA3` `Keccak256`（以太坊）选举产生
- 国密 `SM3`

## MD5算法分析

> 消息摘要（Message Digest）

**产生128比特（16字节）散列值**（ `RFC1321` ）

强抗碰撞已经攻破，但弱抗碰撞（拿到MD5值无法直接反推出原信息）还没有被攻破

**目前已经不安全，若要验证消息的完整性可以使用**（效率高）其余在工程中尽量不使用

为了兼容旧系统，若必须使用 `MD5` 要加 `salt`

### MD5算法原理

- **Step 1：Append Padding Bits**

  对要加密的数据进行填充和填充，**将要加密的总数据二进制数据对 512 取模，得到的结果如果不够 448 位（56字节），则进行补足**，补位的方式就是 第一位填1，后面全部填充 0

​		经过整理完成后的数据位数可以表示为 `N * 512 + 448` （按512位为一块进行处理（64字节））

- **Step 2：Append Length**

  在上述数据后面再追加 64 位用来存储数据的长度，比如说数据的长度为 16 字节，则用 10000 来填充后64位，做完这一步，数据的位数将变为：

  `(N + 1) * 512`

​		此时数据长度就是512的倍数了，`512 / 8 = 64字节 / 4 = 16字节` 接下来我们对这 4 块每块16字节的数据分别做4轮处理（每轮处理4字节数据）

总共进行 4 * 4 * 4 = 64 次运算

- **Step 3：Initialize MD Buffer**

  初始化4个4字节的幻数作为输入 （下面为C++内存中的存储格式）

  ```C
  A = 0x67 45 23 01 // 小端存储
  B = 0xEF CD AB 89 // C++采用小端
  C = 0x98 BA DC FE
  D = 0x10 32 54 76
  ```

  接下来对每16字节分别四轮循环

- **接下来开始对数据做运算**

  如上面我们说的，原文的512位再分成16等份（一份4字节），命名为 `M0 ~ M15` 

![20200416231646254](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210051715279.png)

其中 A、B、C、D 就是上述的4个初始值，每一轮的循环都会产生新的A、B、C、D。

总共进行的循环数 = `(原文处理后的总长度 / 512)  * (512 / 32 *4)`

​	**(原文处理后的总长度 / 512) 为 主循环次数**

#### 图中的 `F`

**在 MD5 算法中官方定义了四个函数** `F` `G` `H` `I`

```C
#define F(b,c,d)        ((((c) ^ (d)) & (b)) ^ (d))
#define G(b,c,d)        ((((b) ^ (c)) & (d)) ^ (c))
#define H(b,c,d)        ((b) ^ (c) ^ (d))
#define I(b,c,d)        (((~(d)) | (b)) ^ (c))
```

在主循环下的64次子循环中，`F` `G` `H` `I` 函数交替使用：

- 第一个16次使用 `F` 
- 第二个16次使用 `G`
- 第三个16次使用 `H`
- 第四个16次使用 `I`

<!--对应图中F的位置 -->

#### 图中红色田字

相加

#### Mi

就是上面说的被分成16等份的数据

#### Ki

常量值，64次子循环中每次使用的常量值都不同

#### <<<s

> 循环左移s位

```C
#define ROTATE(a,n)     (((a)<<(n))|(((a)&0xffffffff)>>(32-(n))))

#define R0(a,b,c,d,k,s,t) { \
        a+=((k)+(t)+F((b),(c),(d))); \
        a=ROTATE(a,s); \
        a+=b; };

#define R1(a,b,c,d,k,s,t) { \
        a+=((k)+(t)+G((b),(c),(d))); \
        a=ROTATE(a,s); \
        a+=b; };

#define R2(a,b,c,d,k,s,t) { \
        a+=((k)+(t)+H((b),(c),(d))); \
        a=ROTATE(a,s); \
        a+=b; };

#define R3(a,b,c,d,k,s,t) { \
        a+=((k)+(t)+I((b),(c),(d))); \
        a=ROTATE(a,s); \
        a+=b; };
```

### 为什么MD5不可逆

- 使用了散列函数，即上面的 `F` `G` `H` `I` 函数
- 大量的移位操作，不可逆

### OpenSSL MD5模块的使用及源码分析

```C
#include <iostream>
#include <openssl/md5.h>

using namespace std;


int main(int argc,char** argv)
{
	unsigned char cData[] = "测试MD5数据";
    unsigned char out[1024];
	int len = sizeof(cData);

	MD5_CTX c;
    
	MD5_Init(&c);
    
    MD5_Update(&c,cData,len); // 进行计算
    
    MD5_Final(out,&c);
    
    for (int i = 0; i < 16; i++)
    {
        cout << hex << (int)out[i]; // hex 格式 以16进制输出
    }
    
    // --------------------------------------------
    // 简化版接口
    MD5(cData,len,out); // 即可

}
```

首先通过 `MD5_CTX c;` 初始化一个 `MD5_CTX` 结构体

```C
typedef struct MD5state_st 
{
	MD5_LONG A, B, C, D;
	MD5_LONG Nl, Nh;
	MD5_LONG data[MD5_LBLOCK];
	unsigned int num;

} MD5_CTX;
```

其中定义了我们上述原理介绍所使用的初始值`A`、`B`、`C`、`D` 等参数

**接下来通过 `MD5_Init(&c)` 初始化该结构体**，观察其源码：

```C
#define INIT_DATA_A (unsigned long)0x67452301L // 小端
#define INIT_DATA_B (unsigned long)0xefcdab89L
#define INIT_DATA_C (unsigned long)0x98badcfeL
#define INIT_DATA_D (unsigned long)0x10325476L

int MD5_Init(MD5_CTX *c)
{
    memset(c, 0, sizeof(*c));
    c->A = INIT_DATA_A;
    c->B = INIT_DATA_B;
    c->C = INIT_DATA_C;
    c->D = INIT_DATA_D;
    return 1;
}
```

可以看出初始化的目的有两个：

1. 将结构体 c 通过 `memset()` 清 0 初始化
2. 将`A B C D`初始化为之前提到过的4个初始值

然后 `MD5_Update(&c,cData,len); // 进行计算`

**会对数据进程初始化的补全与处理，然后按块进行加密**

```C
void md5_block_data_order(MD5_CTX *c, const void *data_, size_t num)
{
    const unsigned char *data = data_;
    register unsigned MD32_REG_T A, B, C, D, l;
# ifndef MD32_XARRAY
    /* See comment in crypto/sha/sha_local.h for details. */
    unsigned MD32_REG_T XX0, XX1, XX2, XX3, XX4, XX5, XX6, XX7,
        XX8, XX9, XX10, XX11, XX12, XX13, XX14, XX15;
#  define X(i)   XX##i
# else
    MD5_LONG XX[MD5_LBLOCK];
#  define X(i)   XX[i]
# endif

    A = c->A;
    B = c->B;
    C = c->C;
    D = c->D;

    for (; num--;) {
        (void)HOST_c2l(data, l);
        X(0) = l;
        (void)HOST_c2l(data, l);
        X(1) = l;
        /* Round 0 */
        R0(A, B, C, D, X(0), 7, 0xd76aa478L);
        (void)HOST_c2l(data, l);
        X(2) = l;
        R0(D, A, B, C, X(1), 12, 0xe8c7b756L);
        (void)HOST_c2l(data, l);
        X(3) = l;
        R0(C, D, A, B, X(2), 17, 0x242070dbL);
        (void)HOST_c2l(data, l);
        X(4) = l;
        R0(B, C, D, A, X(3), 22, 0xc1bdceeeL);
        (void)HOST_c2l(data, l);
        X(5) = l;
        R0(A, B, C, D, X(4), 7, 0xf57c0fafL);
        (void)HOST_c2l(data, l);
        X(6) = l;
        R0(D, A, B, C, X(5), 12, 0x4787c62aL);
        (void)HOST_c2l(data, l);
        X(7) = l;
        R0(C, D, A, B, X(6), 17, 0xa8304613L);
        (void)HOST_c2l(data, l);
        X(8) = l;
        R0(B, C, D, A, X(7), 22, 0xfd469501L);
        (void)HOST_c2l(data, l);
        X(9) = l;
        R0(A, B, C, D, X(8), 7, 0x698098d8L);
        (void)HOST_c2l(data, l);
        X(10) = l;
        R0(D, A, B, C, X(9), 12, 0x8b44f7afL);
        (void)HOST_c2l(data, l);
        X(11) = l;
        R0(C, D, A, B, X(10), 17, 0xffff5bb1L);
        (void)HOST_c2l(data, l);
        X(12) = l;
        R0(B, C, D, A, X(11), 22, 0x895cd7beL);
        (void)HOST_c2l(data, l);
        X(13) = l;
        R0(A, B, C, D, X(12), 7, 0x6b901122L);
        (void)HOST_c2l(data, l);
        X(14) = l;
        R0(D, A, B, C, X(13), 12, 0xfd987193L);
        (void)HOST_c2l(data, l);
        X(15) = l;
        R0(C, D, A, B, X(14), 17, 0xa679438eL);
        R0(B, C, D, A, X(15), 22, 0x49b40821L);
        /* Round 1 */
        R1(A, B, C, D, X(1), 5, 0xf61e2562L);
        R1(D, A, B, C, X(6), 9, 0xc040b340L);
        R1(C, D, A, B, X(11), 14, 0x265e5a51L);
        R1(B, C, D, A, X(0), 20, 0xe9b6c7aaL);
        R1(A, B, C, D, X(5), 5, 0xd62f105dL);
        R1(D, A, B, C, X(10), 9, 0x02441453L);
        R1(C, D, A, B, X(15), 14, 0xd8a1e681L);
        R1(B, C, D, A, X(4), 20, 0xe7d3fbc8L);
        R1(A, B, C, D, X(9), 5, 0x21e1cde6L);
        R1(D, A, B, C, X(14), 9, 0xc33707d6L);
        R1(C, D, A, B, X(3), 14, 0xf4d50d87L);
        R1(B, C, D, A, X(8), 20, 0x455a14edL);
        R1(A, B, C, D, X(13), 5, 0xa9e3e905L);
        R1(D, A, B, C, X(2), 9, 0xfcefa3f8L);
        R1(C, D, A, B, X(7), 14, 0x676f02d9L);
        R1(B, C, D, A, X(12), 20, 0x8d2a4c8aL);
        /* Round 2 */
        R2(A, B, C, D, X(5), 4, 0xfffa3942L);
        R2(D, A, B, C, X(8), 11, 0x8771f681L);
        R2(C, D, A, B, X(11), 16, 0x6d9d6122L);
        R2(B, C, D, A, X(14), 23, 0xfde5380cL);
        R2(A, B, C, D, X(1), 4, 0xa4beea44L);
        R2(D, A, B, C, X(4), 11, 0x4bdecfa9L);
        R2(C, D, A, B, X(7), 16, 0xf6bb4b60L);
        R2(B, C, D, A, X(10), 23, 0xbebfbc70L);
        R2(A, B, C, D, X(13), 4, 0x289b7ec6L);
        R2(D, A, B, C, X(0), 11, 0xeaa127faL);
        R2(C, D, A, B, X(3), 16, 0xd4ef3085L);
        R2(B, C, D, A, X(6), 23, 0x04881d05L);
        R2(A, B, C, D, X(9), 4, 0xd9d4d039L);
        R2(D, A, B, C, X(12), 11, 0xe6db99e5L);
        R2(C, D, A, B, X(15), 16, 0x1fa27cf8L);
        R2(B, C, D, A, X(2), 23, 0xc4ac5665L);
        /* Round 3 */
        R3(A, B, C, D, X(0), 6, 0xf4292244L);
        R3(D, A, B, C, X(7), 10, 0x432aff97L);
        R3(C, D, A, B, X(14), 15, 0xab9423a7L);
        R3(B, C, D, A, X(5), 21, 0xfc93a039L);
        R3(A, B, C, D, X(12), 6, 0x655b59c3L);
        R3(D, A, B, C, X(3), 10, 0x8f0ccc92L);
        R3(C, D, A, B, X(10), 15, 0xffeff47dL);
        R3(B, C, D, A, X(1), 21, 0x85845dd1L);
        R3(A, B, C, D, X(8), 6, 0x6fa87e4fL);
        R3(D, A, B, C, X(15), 10, 0xfe2ce6e0L);
        R3(C, D, A, B, X(6), 15, 0xa3014314L);
        R3(B, C, D, A, X(13), 21, 0x4e0811a1L);
        R3(A, B, C, D, X(4), 6, 0xf7537e82L);
        R3(D, A, B, C, X(11), 10, 0xbd3af235L);
        R3(C, D, A, B, X(2), 15, 0x2ad7d2bbL);
        R3(B, C, D, A, X(9), 21, 0xeb86d391L);

        A = c->A += A; // 每4轮都会改变该值
        B = c->B += B;
        C = c->C += C;
        D = c->D += D;
    }
}
#endif

```

![WeChat Screenshot_20221006120621](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210061206038.png)

## 通过Hash列表验证文件完整性

> Hash List

如果将整个文件全部读入内存再计算Hash值验证文件完整性，那么内存开销过大，所以要分块读取文件，**分块生成Hash值，最终合并所有的Hash值再生成Hash值来验证整个文件是否完整**

<!-- Hash值都是二进制，不是字符串，String类型是可以存放二进制的 -->

```C++
#include <iostream>
#include <openssl/md5.h>
#include <fstream>
#include <thread>
#define __BLOCK_SIZE__	1024	// 一次读取多少字节
#define __BUF_SIZE__	2048
#define __MD5_OUT__		256
using namespace std;
string fnGetFileListHash(string filepath)
{
	string hash;
	// 以二进制的方式打开 以文本方式打开可能只改了格式文本没变化
	ifstream ifs(filepath, ios::binary);
	if (!ifs)
	{
		return hash;
	}

	// 文件读取缓冲Buf
	unsigned char buf[__BUF_SIZE__] = {0};

	// Hash输出
	unsigned char out[__MD5_OUT__] = { 0 };

	while (!ifs.eof()) // 若没读取到文件末尾
	{
		ifs.read((char*)buf, __BLOCK_SIZE__);

		int read_size = ifs.gcount(); // 已读取的数量

		if (read_size <= 0)
		{
			break;
		}

		MD5(buf, read_size, out);

		hash.insert(hash.end(), out, out + 16);
		// MD5后的长度为16字节，所以结尾指针为 out + 16
	}

	ifs.close();

	// 计算所有块哈希的总哈希
	MD5((unsigned char*)hash.data(), hash.size(), out); 

	return string(out, out + 16); // 返回最后的Hash
	
}

void PrintfHEX(string data)
{
	cout << "The Hash of the FILE you indicate is :" << endl;

	for (auto c : data)
	{
		cout << hex << (int)(unsigned char)c; // 若有符号位int强转后会补位
	}

	cout << endl;
}


int main(int argc, char** argv)
{
	unsigned char cData[] = "测试MD5数据";
	unsigned char cOut[1024];
	int len = sizeof(cData);

	MD5_CTX c;
	/*
	typedef struct MD5state_st
	{
		MD5_LONG A, B, C, D;
		MD5_LONG Nl, Nh;
		MD5_LONG data[MD5_LBLOCK];
		unsigned int num;

	} MD5_CTX;
	*/

	MD5_Init(&c);

	MD5_Update(&c, cData, len);

	MD5_Final(cOut, &c);

	cout << "MD5后的数据为：" << endl;
	for (int i = 0; i < 16; i++)
	{
		cout << hex << (int)cOut[i];
	}

	cout << endl;

	string filepath = "../../src/First_OpenSSL/MD5Hash.cpp";

	auto hash_old = fnGetFileListHash(filepath);
	PrintfHEX(hash_old);


	// 验证文件完整性
	for (;;)
	{
		auto hash_new = fnGetFileListHash(filepath);
		
		if (hash_old != hash_new)
		{
			
			cout << "文件被修改" << endl;
			hash_old = hash_new;
			PrintfHEX(hash_new);
		}

		this_thread::sleep_for(1s);
	}
}

	
```

![WeChat Screenshot_20221006130744](https://raw.githubusercontent.com/De4tsh/typoraPhoto/main/img/202210061310810.png)


