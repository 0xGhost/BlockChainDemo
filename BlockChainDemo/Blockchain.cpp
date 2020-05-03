#include "Blockchain.h"

Blockchain::Blockchain(const unsigned int num) : numOf0(num)
{
	chain.emplace_back(Block(0, "StartBlock"));
}

void Blockchain::AddBlock(Block newBlock)
{
	newBlock.SetPrevHash(chain.back().GetHash());
	newBlock.Mine(numOf0);
	chain.push_back(newBlock);
}
