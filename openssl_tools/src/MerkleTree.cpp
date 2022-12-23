string fnGetFileMerkleHash(string filepath)
{
	/*	
	                A               A
                  /  \            /   \
                B     C         B       C
               / \    |        / \     / \
              D   E   F       D   E   F   F
             / \ / \ / \     / \ / \ / \ / \
             1 2 3 4 5 6     1 2 3 4 5 6 5 6
	*/
	string hash;

	// 存放hash叶子节点，后面所有结果都存在其中
	vector<string>hashes;

	ifstream ifs(filepath, ios::binary);
	if (!ifs)
	{
		return hash;
	}

	unsigned char buf[__BLOCK_SIZE__] = { 0 };
	unsigned char out[__BLOCK_SIZE__] = { 0 };

	// int block_size = 256;

	while (!ifs.eof())
	{
		ifs.read((char*)buf, __BLOCK_SIZE__);
		int read_size = ifs.gcount();

		if (read_size <= 0)
		{
			break;
		}

		SHA1(buf, __BLOCK_SIZE__,out);

		// 写入叶子节点的hash值
		hashes.push_back(string(out, out + 20));
	}

	while (hashes.size() > 1) // == 1表示已经计算到root节点了
	{
		// 不是二的倍数补节点（二叉树） // size二进制数最后一位为 1 表示其不是二的倍数
		if (hashes.size() & 1)
		{
			hashes.push_back(hashes.back()); // 将最后一个节点再插入一次用作补充节点
		}


		// 把两两节点的hash结果再写入hashes中
		for (int i = 0; i < hashes.size() / 2; i++)
		{
			// 两个节点拼起来 i表示的是父节点对应图中的D E F F
			string tmp_hash = hashes[i * 2]; // 0 2 4...
			tmp_hash += hashes[i * 2 + 1];   // 1 3 5...

			SHA1((unsigned char*)tmp_hash.data(), tmp_hash.size(),out);

			// 写入结果
			hashes[i] = string(out, out + 20);
		}

		// hash列表删除上一次多余的hash值
		hashes.resize(hashes.size() / 2); // 此时只剩下父节点的hash
		// 再重复循环
	}
	
	if (hashes.size() == 0)
	{
		return hash;
	}

	return hashes[0];

}
