// Yiang Lu 03/05/2020
#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <mutex>
#include "Transaction.h"

using std::string;
using std::vector;

class Block
{
public:
	Block(const unsigned int index);

	void Mine(const unsigned int numOf0, bool print = false);
	bool ThreadingStoppableMine(const unsigned int numOf0, int& newestBlock);
	bool Verify(const unsigned int numOf0) const;

	void AddTransaction(Transaction t, bool resetTime = false);

	unsigned int GetIndex() const	{ return index; }
	string GetTimeStamp()	const	{ return timeStamp; }
	long long GetNonce()	const	{ return nonce; }
	string GetHash()		const	{ return hash; }
	string GetPrevHash()	const	{ return prevBlock == nullptr ? string(64, '0') : prevBlock->GetHash(); }
	vector<Transaction> GetData()		const { return data; }

	void SetPrevBlock(Block& prev) { prevBlock = &prev; }
	void SetIndex(int i)	{ index = i; }

	friend std::ostream& operator<<(std::ostream& out, const Block& b);
	friend std::istream& operator>>(std::istream& in, Block& b);
private:
	unsigned int index;
	string timeStamp;
	long long nonce;
	vector<Transaction> data;
	//string* prevHash;
	Block* prevBlock;
	string hash;
	//vector<Block*> nextBlocks;



	string CombineBlockString() const;
};

