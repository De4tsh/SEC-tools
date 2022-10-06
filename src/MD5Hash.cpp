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

	