// Yiang Lu create by 03/05/2020
#include "Block.h"
#include "SHA256.h"
#include <sstream>
#include <time.h>

Block::Block(const unsigned int index, const string& data) : index(index), data(data)
{
	nonce = 0;
	timeStamp = time(nullptr);
}

void Block::Mine(const unsigned int numOf0)
{
	hash = "1";
	string target(numOf0, '0');
	while (hash.compare(0, numOf0, target) != 0)
	{
		++nonce;
		std::stringstream sstr;
		string blockStr;
		sstr << index << nonce << data << prevHash << timeStamp;
		sstr >> blockStr;
		hash = sha256(blockStr);
	}
}
