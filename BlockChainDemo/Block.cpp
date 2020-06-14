// Yiang Lu 03/05/2020
#include "Block.h"
#include "SHA256.h"
#include <sstream>
#include <time.h>

Block::Block(const unsigned int index) : index(index)
{
	nonce = 0;
	timeStamp = 0;
	prevBlock = nullptr;
	//timeStamp = time(nullptr);
}

void Block::Mine(const unsigned int numOf0, bool p)
{
	hash = "1";
	string target(numOf0, '0');
	while (hash.compare(0, numOf0, target) != 0)
	{
		++nonce; //if(p)std::cout << "c";
		hash = sha256(CombineBlockString());
	}
}

bool Block::ThreadingStoppableMine(const unsigned int numOf0, int& newestBlock)
{
	int miningBlock = newestBlock;
	hash = "1";
	string target(numOf0, '0');
	while (miningBlock == newestBlock && hash.compare(0, numOf0, target) != 0)
	{
		++nonce; //std::cout << "m";
		hash = sha256(CombineBlockString());
	}

	return Verify(numOf0);
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
	sstr << GetPrevHash() << timeStamp;
	sstr >> blockStr;
	return blockStr;
}

std::ostream& operator<<(std::ostream& out, const Block& b) 
{
	out << "index: " << b.index << " "
		<< "timeStamp: " << b.timeStamp << " "
		<< "nonce: " << b.nonce 
		<< "\ndata:\n";
		for (Transaction t : b.data)
		{
			out << t << "\n";
		}
		out << "prev: " << b.GetPrevHash() << "\n"
		<< "hash: " << b.hash << "\n";
	return out;
}

std::istream& operator>>(std::istream& in, Block& b)
{
	string temp;
	in >> temp >> b.index
		>> temp >> b.timeStamp
		>> temp >> b.nonce >> temp;
	char a;
	//in >> a; // skip '\n'
	a = (in >> std::ws).peek();
	while ((in >> std::ws).peek() == 'T')
	{
		Transaction t;
		in >> t;
		b.data.emplace_back(t);
		//in >> a;
	}
	in >> temp >> temp
		>> temp >> b.hash;
	return in;
}
