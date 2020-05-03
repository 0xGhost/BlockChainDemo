// Yiang Lu create by 03/05/2020
#pragma once
#include "Block.h"
#include <vector>

using std::vector;

class Blockchain
{
public:
	Blockchain(const unsigned int num = 0);

	void AddBlock(Block newBlock);
	const vector<Block> GetChain() const { return chain; }

private:
	vector<Block> chain;
	unsigned int numOf0;
};

