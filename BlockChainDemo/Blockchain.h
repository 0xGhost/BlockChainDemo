// Yiang Lu 03/05/2020
#pragma once
#include "Block.h"
#include <vector>

using std::vector;

class Blockchain
{
public:
	Blockchain(const unsigned int num = 4);
	~Blockchain() { }

	void AddBlock(Block* newBlock);
	const vector<Block*> GetChain() const { return chain; }
	
	friend std::ostream& operator<<(std::ostream& out, const Blockchain& bc);
	friend std::istream& operator>>(std::istream& in, Blockchain& bc);


private:
	vector<Block*> chain;
	unsigned int numOf0;
};

