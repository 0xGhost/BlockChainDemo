// Yiang Lu 03/05/2020
#include "Transaction.h"
#include "Player.h"
#include "RSATool.h"
#include <iostream>
#include <sstream>

Transaction::Transaction(Player* oldOwner, Player* newOwner, string item) 
	: oldOwner(oldOwner), newOwner(newOwner), item(item) 
{
}

bool Transaction::Verify() const
{
	return RSATool::VerifySignature(oldOwner->GetPublicKey(), GetMessage(), signature);
}

string Transaction::GetMessage() const
{
	if (item.size() <= 0)
	{
		return "No Transaction";
	}
	std::stringstream sstr;
	string message;
	sstr << oldOwner->GetName() << newOwner->GetName() << item;
	sstr >> message;
	return message;
}

std::ostream& operator<<(std::ostream& out, const Transaction& t)
{
	if (t.item.size() <= 0)
	{
		out << "No Transaction";
	}
	else
	{
		out << t.oldOwner->GetName() << " gives " << t.item << " to " << t.newOwner->GetName() << ".  Authentic: " << std::boolalpha <<  t.Verify();
	}
	return out;
}