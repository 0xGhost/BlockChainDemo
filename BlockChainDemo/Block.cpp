// Yiang Lu 03/05/2020
#include "Block.h"
#include "SHA256.h"
#include <sstream>
#include <time.h>

Block::Block(const unsigned int index) : index(index)
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
		hash = sha256(CombineBlockString());
	}
}

bool Block::Verify(const unsigned int numOf0) const
{
	string target(numOf0, '0');
	string hash = sha256(CombineBlockString());
	return hash.compare(0, numOf0, target) == 0;
}

void Block::AddTransaction(const Transaction t)
{
	data.push_back(t);
}

string Block::CombineBlockString() const
{
	std::stringstream sstr;
	string blockStr;
	sstr << index << nonce;
	for (Transaction t : data)
		sstr << t.GetMessage();
	sstr << prevHash << timeStamp;
	sstr >> blockStr;
	return blockStr;
}

std::ostream& operator<<(std::ostream& out, const Block& b) 
{
	out << "index: " << b.index << " "
		<< "timeStamp: " << b.timeStamp << " "
		<< "nonce: " << b.nonce << "\ndata:\n";
		for (Transaction t : b.data)
		{
			out << t << "\n";
		}
		
		out << "prev: " << b.prevHash << "\n"
		<< "hash: " << b.hash << "\n";
	return out;
}