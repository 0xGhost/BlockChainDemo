// Yiang Lu 05/05/2020
#pragma once
#include <string>
#include "MSTimeStamp.h"

using std::string;
class Player;
class Transaction
{
public:
	Transaction() { item = ""; }
	Transaction(Player* oldOwner, Player* newOwner, string item);
		

	bool Verify() const;

	Player* GetOldOwner()	const	{ return oldOwner; }
	Player* GetNewOwner()	const	{ return newOwner; }
	string	GetItem()		const	{ return item; }
	TimeStamp GetTimeStamp() const	{ return timeStamp; }

	string GetMessageString() const;

	void ResetTimeStamp(); // only for test
	void SetSignature(char* s) { signature = s; }

	friend std::ostream& operator<<(std::ostream& out, const Transaction& b);
	friend std::istream& operator>>(std::istream& out, Transaction& b);

private:
	Player *oldOwner = nullptr;
	Player *newOwner = nullptr;
	TimeStamp timeStamp;
	string item;
	char* signature;
};

