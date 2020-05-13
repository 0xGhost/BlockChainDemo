// Yiang Lu 03/05/2020
#include "Blockchain.h"

Blockchain::Blockchain(const unsigned int num) : numOf0(num)
{
	Block *b = new Block(0);
	b->SetPrevHash(string(64, '0'));
	b->Mine(numOf0);
	chain.push_back(b);
}

void Blockchain::AddBlock(Block* newBlock)
{
	newBlock->SetPrevHash(chain.back()->GetHash());
	newBlock->Mine(numOf0);
	chain.push_back(newBlock);
}
