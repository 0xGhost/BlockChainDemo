#include "NetworkNode.h"
#include <sstream>
extern const int NOZ;


NetworkNode::NetworkNode(const int port)
{
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
	message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_USER_PACKET_ENUM));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	//this->rakPeer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
	//	RakNet::UNASSIGNED_SYSTEM_ADDRESS, false);
	
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	//rakPeer->Send(message.c_str(), message.length() + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

}

void NetworkNode::SendTransactions(const Transaction t)
{
	std::stringstream ss;
	ss << t;
	string message = ss.str();
	if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	{
		std::cout << "Warning: Transaction too long. TODO: Implement large packet transfer." << std::endl;
		message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	}

	RakNet::BitStream bs;

	bs.Write(static_cast<unsigned char>(ID_NEW_TRANSACTION));
	bs.Write(static_cast<unsigned int>(message.size()));
	bs.Write(message.c_str(), static_cast<unsigned int>(message.size()));

	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

void NetworkNode::SendBlockchain(RakNet::RakNetGUID guid)
{
	std::stringstream ss;
	ss << *blockchain;
	string message = ss.str();
	if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	{
		std::cout << "Warning: Blockchain too large. TODO: Implement large packet transfer." << std::endl;
		message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	}

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
	if (message.size() > this->MAX_USER_MESSAGE_LENGTH)
	{
		std::cout << "Warning: Blockchain too large. TODO: Implement large packet transfer." << std::endl;
		message = message.substr(0, this->MAX_USER_MESSAGE_LENGTH);
	}

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
	char* message = new char[this->MAX_USER_MESSAGE_LENGTH + 1];
	
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
				unsigned char rcv_id;
				bts.Read(rcv_id);
				unsigned int length;
				bts.Read(length);
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
			case ID_BLOCKCHAIN_DATA:
			{
				Blockchain* newChain = new Blockchain();
				unsigned char rcv_id;
				bts.Read(rcv_id);
				unsigned int length;
				bts.Read(length);
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
		} // package receive loop
	} // listening loop

	delete[] message;
}
