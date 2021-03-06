#include "NetworkNode.h"
#include <sstream>

extern const int NOZ;
extern const int NOT;


NetworkNode::NetworkNode(const int port)
{
	std::cout << "Initializing blockchain..." << std::endl;

	blockchain = new Blockchain(NOZ);

	this->rakPeer = RakNet::RakPeerInterface::GetInstance();
	rakPeer->SetIncomingPassword("BlockChainDemo", (int)strlen("BlockChainDemo"));
	rakPeer->SetTimeoutTime(30000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	RakNet::SocketDescriptor* socketDescriptor;
	if (port < 0)
		socketDescriptor = new RakNet::SocketDescriptor();
	else
		socketDescriptor = new RakNet::SocketDescriptor(port, 0);
	socketDescriptor->socketFamily = AF_INET;
	this->rakPeer->Startup(MAX_CONNECTION, socketDescriptor, 1);
	rakPeer->SetOccasionalPing(true);
	this->rakPeer->SetMaximumIncomingConnections(MAX_CONNECTION);
	this->GUID = this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS);

	std::cout << "* Your GUID is:" << std::endl << this->GUID.ToString() << std::endl;

	// Start the thread for packet receiving
	this->isListening = true;
	this->listenLoopThread = new std::thread(&NetworkNode::ListenLoop, this);

	//RakNet::ConnectionAttemptResult result = rakPeer->Connect(this->natPunchServerAddress->ToString(false),
	//	this->natPunchServerAddress->GetPort(), 0, 0);
}

NetworkNode::~NetworkNode()
{
	delete blockchain;
	// Stops all networking threads of the RakNet, blocks for a maximum 1000ms
	this->rakPeer->Shutdown(1000);

	// Terminate the thread for packet receiving
	this->isListening = false;
	this->listenLoopThread->join();
	delete this->listenLoopThread;

	// Cleanup...
	RakNet::RakPeerInterface::DestroyInstance(this->rakPeer);
	delete this->packet;
}

bool NetworkNode::CollectNewTransaction(const Transaction t)
{
	if (t.Verify())
	{
		TimeStamp* lastTransactionTime = nullptr;
		if (pendingTransactions.size() > 0)
		{
			lastTransactionTime = &pendingTransactions.back().GetTimeStamp();
		}

		/*if (pendingBlock == nullptr)
		{
			pendingBlock = new Block(blockchain->GetChain().size());

			if (blockchain->GetChain().back()->GetData().size() > 0)
			{
				lastTransactionTime = &(blockchain->GetChain().back()->GetData().back().GetTimeStamp());
			}
		}
		else
		{
			lastTransactionTime = &pendingBlock->GetData().back().GetTimeStamp();
		}*/


		//if (pendingBlock->GetData().size() >= NOT)
		//{
		//	std::cout << "New block have enough transactions. Mining...\n";
		//	pendingBlock->SetIndex(blockchain->GetChain().size());
		//	blockchain->AddBlock(pendingBlock);
		//	std::cout << "New block mined. Boardcast new block.\n";
		//	SendBlock(*pendingBlock);
		//	pendingBlock = new Block(blockchain->GetChain().size());
		//}

		if (lastTransactionTime != nullptr)
		{
			if (t.GetTimeStamp().compare(*lastTransactionTime) <= 0)
			{
				std::cout << "Skip old Transaction. \n";
				return false;
			}
		}
		
		//pendingBlock->AddTransaction(t);

		pendingTransactions.push_back(t);

		std::cout << "New transaction added into pending transaction queue.\n";
		return true;
		//SendTransaction(t);
	}
	else
	{
		std::cout << "Transaction not verified." << std::endl;
		return false;
	}
}

void NetworkNode::PendingBlockCheck()
{
	if (pendingTransactions.size() >= 20)
	{
		pendingBlock = new Block(blockchain->GetChain().size());
		for (int i = 0; i < NOT; i++)
		{
			pendingBlock->AddTransaction(pendingTransactions.back());
			pendingTransactions.pop_back();
		}
		std::cout << "Pending block have enough transactions. Mining...\n";
		pendingBlock->SetIndex(blockchain->GetChain().size());
		blockchain->AddBlock(pendingBlock);

		std::cout << "Pending block mined. Boardcast new block.\n";
		SendBlock(*pendingBlock);
	}

	/*if (pendingBlock != nullptr && pendingBlock->GetData().size() >= NOT)
	{
		std::cout << "New block have enough transactions. Mining...\n";
		pendingBlock->SetIndex(blockchain->GetChain().size());
		blockchain->AddBlock(pendingBlock);
		std::cout << "New block mined. Boardcast new block.\n";
		SendBlock(*pendingBlock);
		pendingBlock = new Block(blockchain->GetChain().size());
	}*/
}

void NetworkNode::Connect(const int port)
{
	RakNet::ConnectionAttemptResult car = rakPeer->Connect("127.0.0.1", port, "BlockChainDemo", (int)strlen("BlockChainDemo"));
	if (car == RakNet::CONNECTION_ATTEMPT_STARTED) return;
	if (car == RakNet::ALREADY_CONNECTED_TO_ENDPOINT) std::cout << "Already connected to: " << port << std::endl;
	else std::cout << "Failed to connected port: " << port << "    Result: " << car << std::endl;
	// TODO: handle all the Result
}

void NetworkNode::SendStringMessage(string message)
{
	// Just trim the message, just in case.
	//message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_USER_PACKET_ENUM));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	//this->rakPeer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
	//	RakNet::UNASSIGNED_SYSTEM_ADDRESS, false);
	
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	//rakPeer->Send(message.c_str(), message.length() + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

}

void NetworkNode::SendTransaction(const Transaction t)
{
	std::stringstream ss;
	ss << t;
	string message = ss.str();
	//if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	//{
	//	std::cout << "Warning: Transaction too long. TODO: Implement large packet transfer." << std::endl;
	//	message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	//}

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_NEW_TRANSACTION));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkNode::SendBlock(const Block b)
{
	std::stringstream ss;
	ss << b;
	string message = ss.str();
	//if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	//{
	//	std::cout << "Warning: Block too large. TODO: Implement large packet transfer." << std::endl;
	//	message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	//}

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_ADD_NEW_BLOCK));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkNode::SendBlockchain(RakNet::RakNetGUID guid)
{
	std::stringstream ss;
	ss << *blockchain;
	string message = ss.str();
	//if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	//{
	//	std::cout << "Warning: Blockchain too large. TODO: Implement large packet transfer." << std::endl;
	//	message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	//}

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_BLOCKCHAIN_DATA));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid, false);

}

void NetworkNode::SendBlockchain()
{
	std::stringstream ss;
	ss << *blockchain;
	string message = ss.str();
	//if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	//{
	//	std::cout << "Warning: Blockchain too large. TODO: Implement large packet transfer." << std::endl;
	//	message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	//}

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_BLOCKCHAIN_DATA));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkNode::SendRequestForLatestBlockchain()
{
	RakNet::BitStream bs;
	string message;
	bs.Write(static_cast<unsigned char>(ID_REQUEST_BLOCKCHAIN));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkNode::ListenLoop()
{
	// Allocate the buffer for the incoming message string
	char* message;
	
	while (this->isListening)
	{
		for (this->packet = this->rakPeer->Receive();
			this->packet;
			this->rakPeer->DeallocatePacket(this->packet),
			this->packet = this->rakPeer->Receive())
		{
			RakNet::BitStream bts(this->packet->data,
				this->packet->length,
				false);
			unsigned char rcv_id;
			bts.Read(rcv_id);
			unsigned int length;
			bts.Read(length);
			message = new char[length + 1];

			// Check the packet identifier
			switch (this->packet->data[0])
			{
			case ID_CONNECTION_REQUEST_ACCEPTED:
				printf("ID_CONNECTION_REQUEST_ACCEPTED to %s with GUID %s\n", this->packet->systemAddress.ToString(true), this->packet->guid.ToString());
				printf("My external address is %s\n", this->rakPeer->GetExternalID(this->packet->systemAddress).ToString(true));
				break;
			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "* A peer has connected." << std::endl;
				break;
			
			case ID_USER_PACKET_ENUM:
				//unsigned char rcv_id;
				//bts.Read(rcv_id);
				//unsigned int length;
				//bts.Read(length);
				//if (length >= this->MAX_USER_MESSAGE_LENGTH)
				//{
				//	delete[] message;
				//	message = new char[length + 1];
				//}
				bts.Read(message, length);
				message[length] = '\0';
				std::cout << "* Message received:" << std::endl <<
					message << std::endl;
				break;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				std::cout << "* Remote peer has disconnected." << std::endl;
				break;
			case ID_REMOTE_CONNECTION_LOST:
				std::cout << "* Remote peer lost connection." << std::endl;
				break;
			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "* A peer has disconnected." << std::endl;
				break;
			case ID_CONNECTION_LOST:
				std::cout << "* A connection was lost." << std::endl;
				break;

			case ID_REQUEST_BLOCKCHAIN:
				std::cout << "* ID_REQUEST_BLOCKCHAIN from " << this->packet->systemAddress.ToString(true)
					<< " with GUID:" << this->packet->guid.ToString() << std::endl;
				SendBlockchain(this->packet->guid);
				break;
			case ID_ADD_NEW_BLOCK:
			{
				//unsigned char rcv_id;
				//bts.Read(rcv_id);
				//unsigned int length;
				//bts.Read(length);
				bts.Read(message, length);
				message[length] = '\0';
				std::cout << "* ID_ADD_NEW_BLOCK from " << this->packet->systemAddress.ToString(true)
					<< " with GUID:" << this->packet->guid.ToString() << std::endl;
				Block* b = new Block(0);
				std::stringstream ss;
				ss << message;
				ss >> *b;
				if (b->GetIndex() >= blockchain->GetChain().size())
				{
					if (b->GetIndex() == blockchain->GetChain().size())
					{
						b->SetPrevBlock(*blockchain->GetChain().back());
						if (b->Verify(NOZ))
						{
							blockchain->AddBlock(b);
							std::cout << "new block added into blockchain\n";
							SendBlock(*b);
						}
						else
						{
							std::cout << "new block not pass verify.\n";
						}
					}
					else
					{
						std::cout << "current blockchain out of date. Please request for the latest one.\n";
					}
				}
				else
				{
					std::cout << "old block ignored\n";
				}
			}
				break;
			case ID_NEW_TRANSACTION:
			{
				//unsigned char rcv_id;
				//bts.Read(rcv_id);
				//unsigned int length;
				//bts.Read(length);
				bts.Read(message, length);
				message[length] = '\0';
				std::cout << "* ID_NEW_TRANSACTION from " << this->packet->systemAddress.ToString(true)
					<< " with GUID:" << this->packet->guid.ToString() << std::endl;
				Transaction t;
				std::stringstream ss;
				ss << message;
				ss >> t;
				if(CollectNewTransaction(t)) 
					SendTransaction(t);
			}
				break;
			case ID_BLOCKCHAIN_DATA:
			{
				Blockchain* newChain = new Blockchain(NOZ);
				//unsigned char rcv_id;
				//bts.Read(rcv_id);
				//unsigned int length;
				//bts.Read(length);
				bts.Read(message, length);
				message[length] = '\0';
				std::cout << "* Blockchain received from " << this->packet->systemAddress.ToString(true)
					<< " with GUID:" << this->packet->guid.ToString() << std::endl;

				std::stringstream ss;
				ss << message;
				ss >> *newChain;
				bool verify = true;
				for (Block* b : newChain->GetChain())
				{
					bool result = b->Verify(NOZ);
					std::cout << "Verify block" << b->GetIndex() << ": " << result << std::endl;
					if (!result)
					{
						verify = false;
					}
				}
				if (verify && newChain->GetChain().size() > blockchain->GetChain().size())
				{
					delete blockchain;
					blockchain = newChain;
				}
			}
				break;
			default:
				std::cout << "* Received a packet with unspecified " <<
					"message identifier." << std::endl;
				std::cout << packet->data << std::endl;
				break;
			} // check package identifier
			delete[] message;
		} // package receive loop
	} // listening loop

}
