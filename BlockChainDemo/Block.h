// Yiang Lu create by 03/05/2020
#pragma once
#include<string>

using std::string;

class Block
{
public:
	Block(const unsigned int index, const string& data);

	void Mine(const unsigned int numOf0);

	string GetHash()		const	{ return hash; }
	string GetPrevHash()	const	{ return prevHash; }
	void SetPrevHash(string prev) { prevHash = prev; }
	
private:
	unsigned int index;
	time_t timeStamp;
	long long nonce;
	string data; // TODO: struct? class with GetHash()?
	string prevHash;
	string hash;
};

