// Yiang Lu 03/05/2020
#pragma once
#include <string>
#include <iostream>
#include "Transaction.h"

using std::string;

class Block
{
public:
	Block(const unsigned int index, const Transaction& data);

	void Mine(const unsigned int numOf0);
	bool Verify(const unsigned int numOf0) const;

	unsigned int GetIndex() const	{ return index; }
	time_t GetTimeStamp()	const	{ return timeStamp; }
	long long GetNonce()	const	{ return nonce; }
	Transaction GetData()		const	{ return data; }
	string GetHash()		const	{ return hash; }
	string GetPrevHash()	const	{ return prevHash; }

	void SetPrevHash(string prev)	{ prevHash = prev; }

	friend std::ostream& operator<<(std::ostream& out, const Block& b);
private:
	unsigned int index;
	time_t timeStamp;
	long long nonce;
	Transaction data; // TODO: struct? class with GetHash()?
	string prevHash;
	string hash;

	// TODO: protected string CombineBlockString() const;
};

