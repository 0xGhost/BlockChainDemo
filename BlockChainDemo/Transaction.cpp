// Yiang Lu 03/05/2020
#include "Transaction.h"
#include "Player.h"
#include "RSATool.h"
#include <iostream>
#include <sstream>
#include <Windows.h>

Transaction::Transaction(Player* oldOwner, Player* newOwner, string item) 
	: oldOwner(oldOwner), newOwner(newOwner), item(item) 
{
	timeStamp = TimeStamp();
}

bool Transaction::Verify() const
{
	return RSATool::VerifySignature(oldOwner->GetPublicKey(), GetMessageString(), signature);
}

string Transaction::GetMessageString() const
{
	if (item.size() <= 0)
	{
		return "No Transaction";
	}
	std::stringstream sstr;
	string message;
	sstr << oldOwner->GetName() << newOwner->GetName() << item << timeStamp.ToString();
	sstr >> message;
	return message;
}

void Transaction::RestTimeStamp()
{
	timeStamp = TimeStamp();
	oldOwner->Sign(*this); // fake sign
	Sleep(1);
}

std::ostream& operator<<(std::ostream& out, const Transaction& t)
{
	if (t.item.size() <= 0)
	{
		out << "No Transaction";
	}
	else
	{
		out << "Tx: " << t.oldOwner->GetName() << " gives " << t.item << " to " << t.newOwner->GetName() << " time: " << t.timeStamp.ToString() << "    Authentic: " << std::boolalpha << t.Verify();// << "\nSignature:" << t.signature;
		//if (!t.Verify())
		//{
		//	out << " signature:" << t.signature;
		//}
	}
	return out;
}

std::istream& operator>>(std::istream& in, Transaction& t)
{
	string temp, oldOwnerName, newOwnerName;
	in >> temp >> oldOwnerName >> temp >> t.item >> temp >> newOwnerName >> temp >> t.timeStamp.ToString() >> temp >> temp;
	t.oldOwner = new Player(oldOwnerName);
	t.newOwner = new Player(newOwnerName);
	t.oldOwner->Sign(t); // fake sign
	// TODO: the owner should store public key
	return in;
}
