// Yiang Lu 03/05/2020
#pragma once
#include <string>
#include <iostream>
#include <vector>
#include "Transaction.h"

using std::string;
using std::vector;

class Block
{
public:
	Block(const unsigned int index);

	void Mine(const unsigned int numOf0);
	bool Verify(const unsigned int numOf0) const;

	void AddTransaction(const Transaction t);
	unsigned int GetIndex() const	{ return index; }
	time_t GetTimeStamp()	const	{ return timeStamp; }
	long long GetNonce()	const	{ return nonce; }
	vector<Transaction> GetData()		const	{ return data; }
	string GetHash()		const	{ return hash; }
	string GetPrevHash()	const	{ return prevHash; }

	void SetPrevHash(string prev)	{ prevHash = prev; }

	friend std::ostream& operator<<(std::ostream& out, const Block& b);
private:
	unsigned int index;
	time_t timeStamp;
	long long nonce;
	vector<Transaction> data; // TODO: vector
	string prevHash;
	string hash;


	string CombineBlockString() const;
};

