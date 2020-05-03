// Yiang Lu create by 03/05/2020

#include <iostream>
#include "Blockchain.h"
using namespace std;

int main()
{
	Blockchain blockChain = Blockchain(4);

	cout << "Mining block 1..." << endl;
	blockChain.AddBlock(Block(1, "Block 1 Data"));
	cout << "hash: " << blockChain.GetChain().back().GetHash() << endl;

	cout << "Mining block 2..." << endl;
	blockChain.AddBlock(Block(2, "Block 2 Data"));
	cout << "hash: " << blockChain.GetChain().back().GetHash() << endl;

	cout << "Mining block 3..." << endl;
	blockChain.AddBlock(Block(3, "Block 3 Data"));
	cout << "hash: " << blockChain.GetChain().back().GetHash() << endl;
}
