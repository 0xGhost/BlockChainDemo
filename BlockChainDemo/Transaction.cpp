#include "Transaction.h"
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Transaction& t)
{
	if (t.item.size() <= 0)
	{
		out << "No Transaction";
	}
	else
	{
		out << t.oldOwner << " gives " << t.item << " to " << t.newOwner;
	}
	return out;
}
