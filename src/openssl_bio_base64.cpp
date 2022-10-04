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



//int main()
//{
//
//
//	unsigned char data[] = "true 最终答案在 [left,mid] 区间中 right = mid";
//	char outdata[__BUF_SIZE__] = {0};
//	unsigned char outdata2[__BUF_SIZE__] = { 0 };
//	int len = sizeof(data);
//	size_t re;
//
//	int nLength = Base64Encode(data, len, outdata);
//
//	if (nLength > 0)
//	{
//		outdata[nLength] = '\0';
//	}
//
//	cout << "编码后的数据为：";
//	cout << outdata << endl;
//
//	re = Base64Decode(outdata, nLength, outdata2);
//
//	cout << "解码了：" << re << "个字节，原文为：" << outdata2 << endl;
//
//	return 0;
//
//
//}