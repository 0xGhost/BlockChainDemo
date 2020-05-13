// Yiang Lu 05/05/2020
#pragma once
#include <string>

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

	string GetMessage() const;

	void SetSignature(char* s) { signature = s; }

	friend std::ostream& operator<<(std::ostream& out, const Transaction& b);

private:
	Player *oldOwner = nullptr;
	Player *newOwner = nullptr;
	string item;
	char* signature;
};

