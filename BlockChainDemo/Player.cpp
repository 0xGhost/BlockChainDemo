#include "Player.h"
#include "RSATool.h"
#include <sstream>

void Player::Sign(Transaction& t)
{

	t.SetSignature(GenerateSignature(t.GetMessage()));
}

char* Player::GenerateSignature(string message)
{
	return RSATool::SignMessage(privateKey, message);
}
