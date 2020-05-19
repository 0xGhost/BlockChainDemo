// Yiang Lu 03/05/2020
#include "Blockchain.h"

Blockchain::Blockchain(const unsigned int num) : numOf0(num)
{
	Block *b = new Block(0);
	b->Mine(numOf0);
	chain.push_back(b);
}

void Blockchain::AddBlock(Block* newBlock)
{
	newBlock->SetPrevBlock(*chain.back());
	newBlock->Mine(numOf0);
	chain.push_back(newBlock);
}

std::ostream& operator<<(std::ostream& out, const Blockchain& bc)
{
	for (const Block* b : bc.chain)
	{
		out << *b << std::endl;
	}
	return out;
}

std::istream& operator>>(std::istream& in, Blockchain& bc)
{
	bc.chain.clear();
	while (!in.eof())
	{
		Block* b = new Block(0);
		in >> *b;
		if (!b->GetHash().empty())
		{
			if(b->GetIndex() > 0)
				b->SetPrevBlock(*bc.chain.back());
			bc.chain.push_back(b);
		}
	}
	return in;
}
