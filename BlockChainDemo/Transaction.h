// Yiang Lu 05/05/2020
#pragma once
#include <string>

using std::string;

class Transaction
{
public:
	Transaction() {}
	Transaction(string oldOwner, string newOwner, string item) : oldOwner(oldOwner), newOwner(newOwner), item(item) {}
	//Transaction(User& oldOwner, User& newOwner, Item& item);
	friend std::ostream& operator<<(std::ostream& out, const Transaction& b);
private:
	string oldOwner;
	string newOwner;
	string item;

	//string signature;
};

