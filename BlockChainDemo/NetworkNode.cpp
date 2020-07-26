#include "NetworkNode.h"

NetworkNode::NetworkNode()
{
	this->rakPeer = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor socketDescriptor;
	socketDescriptor.socketFamily = AF_INET;
	this->rakPeer->Startup(MAX_CONNECTION, &socketDescriptor, 1);
	rakPeer->SetMaximumIncomingConnections(10);
	rakPeer->SetOccasionalPing(true);

	std::cout << "* Your GUID is:" << std::endl <<
		this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString()
		<< std::endl;

	// Start the thread for packet receiving
	this->isListening = true;
	this->listenLoopThread = new std::thread(&NetworkNode::ListenLoop, this);
}

NetworkNode::NetworkNode(const int port)
{
	this->rakPeer = RakNet::RakPeerInterface::GetInstance();
	rakPeer->SetIncomingPassword("BlockChainDemo", (int)strlen("BlockChainDemo"));
	rakPeer->SetTimeoutTime(30000, RakNet::UNASSIGNED_SYSTEM_ADDRESS);
	RakNet::SocketDescriptor socketDescriptor(port,0);
	socketDescriptor.socketFamily = AF_INET;
	this->rakPeer->Startup(MAX_CONNECTION, &socketDescriptor, 1);
	rakPeer->SetOccasionalPing(true);
	this->rakPeer->SetMaximumIncomingConnections(MAX_CONNECTION);

	//std::cout << "* Your GUID is:" << std::endl <<
	//	this->rakPeer->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString()
	//	<< std::endl;


	// Start the thread for packet receiving
	this->isListening = true;
	this->listenLoopThread = new std::thread(&NetworkNode::ListenLoop, this);

	//RakNet::ConnectionAttemptResult result = rakPeer->Connect(this->natPunchServerAddress->ToString(false),
	//	this->natPunchServerAddress->GetPort(), 0, 0);
}

NetworkNode::~NetworkNode()
{
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
	RakAssert(car == RakNet::CONNECTION_ATTEMPT_STARTED);
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
	
	//rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	rakPeer->Send(message.c_str(), message.length() + 1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

}

void NetworkNode::ListenLoop()
{
	// Allocate the buffer for the incoming message string
	char* message = new char[this->MAX_USER_MESSAGE_LENGTH];
	
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
