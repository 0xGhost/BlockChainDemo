// Yiang Lu create by 03/05/2020
#pragma once
#include <string>
#include <iostream>

using std::string;

class Block
{
public:
	Block(const unsigned int index, const string& data);

	void Mine(const unsigned int numOf0);

	unsigned int GetIndex() const	{ return index; }
	time_t GetTimeStamp()	const	{ return timeStamp; }
	long long GetNonce()	const	{ return nonce; }
	string GetData()		const	{ return data; }
	string GetHash()		const	{ return hash; }
	string GetPrevHash()	const	{ return prevHash; }

	void SetPrevHash(string prev)	{ prevHash = prev; }

	friend std::ostream& operator<<(std::ostream& out, const Block& b) {
		out << "index: " << b.index << " "
			<< "timeStamp: " << b.timeStamp << " "
			<< "nonce: " << b.nonce << "\n"
			<< "data: " << b.data << "\n"
			<< "prev: " << b.prevHash << "\n"
			<< "hash: " << b.hash << "\n";
		return out;
	}
private:
	unsigned int index;
	time_t timeStamp;
	long long nonce;
	string data; // TODO: struct? class with GetHash()?
	string prevHash;
	string hash;

	// TODO: protected string CombineBlockString() const;
};

