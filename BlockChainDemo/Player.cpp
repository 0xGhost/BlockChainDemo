#include "Player.h"
#include "RSATool.h"
#include <sstream>

void Player::Sign(Transaction& t) const
{
	t.SetSignature(GenerateSignature(t.GetMessageString()));
}

char* Player::GenerateSignature(string message) const
{
	return RSATool::SignMessage(privateKey, message);
}
