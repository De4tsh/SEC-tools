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
