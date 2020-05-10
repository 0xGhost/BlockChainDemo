// Yiang Lu 03/05/2020
#include "Block.h"
#include "SHA256.h"
#include <sstream>
#include <time.h>

Block::Block(const unsigned int index, const Transaction& data) : index(index), data(data)
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
		sstr << index << nonce << data.GetMessage() << prevHash << timeStamp;
		sstr >> blockStr;
		hash = sha256(blockStr);
	}
}

bool Block::Verify(const unsigned int numOf0) const
{
	string target(numOf0, '0');
	std::stringstream sstr;
	string blockStr;
	sstr << index << nonce << data.GetMessage() << prevHash << timeStamp;
	sstr >> blockStr;
	string hash = sha256(blockStr);
	return hash.compare(0, numOf0, target) == 0;
}

std::ostream& operator<<(std::ostream& out, const Block& b) 
{
	out << "index: " << b.index << " "
		<< "timeStamp: " << b.timeStamp << " "
		<< "nonce: " << b.nonce << "\n"
		<< "data: " << b.data << "\n"
		<< "prev: " << b.prevHash << "\n"
		<< "hash: " << b.hash << "\n";
	return out;
}