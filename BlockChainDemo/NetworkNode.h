#pragma once

#include <string>
#include <thread>
#include <iostream>
// Raknet includes
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "Blockchain.h"

using std::string;

class NetworkNode
{
public:

	NetworkNode(const int port = -1); // -1 means using random port
	~NetworkNode();
	

	void Connect(const int port);

	void SendStringMessage(string message);

	void SendTransactions(const Transaction t);
	void SendBlockchain(RakNet::RakNetGUID guid);
	void SendBlockchain();
	void SendRequestForLatestBlockchain();

	Blockchain* GetBlockchain() { return blockchain; }

	void SetBlockchain(Blockchain* bc) { delete blockchain; blockchain = bc; }

private:
	// The maximum length for the string messages sent between the peers.
	const int MAX_USER_MESSAGE_LENGTH = 4096;
	const int MAX_CONNECTION = 10;

	RakNet::RakPeerInterface* rakPeer;
	RakNet::Packet* packet;
	RakNet::RakNetGUID GUID;

	Blockchain *blockchain;

	// The C++11 thread for receiving packets in the background.
	std::thread* listenLoopThread;
	// Listens for incoming packets and processes them according to their
	// message identifiers.
	void ListenLoop();
	// The flag for breaking the loop inside the packet listening thread.
	bool isListening;

	

};

