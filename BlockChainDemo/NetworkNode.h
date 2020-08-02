#pragma once
#include <queue>
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
	
	bool CollectNewTransaction(const Transaction t);
	void PendingBlockCheck();

	void Connect(const int port);

	void SendStringMessage(string message);

	void SendTransaction(const Transaction t);
	void SendBlock(const Block b);
	void SendBlockchain(RakNet::RakNetGUID guid);
	void SendBlockchain();
	void SendRequestForLatestBlockchain();

	Blockchain* GetBlockchain() const { return blockchain; }
	Block* GetPendingBlock() const { return pendingBlock; }
	std::deque<Transaction> GetPendingTransactions() const { return pendingTransactions; }

	void SetBlockchain(Blockchain* bc) { delete blockchain; blockchain = bc; }

private:
	// The maximum length for the string messages sent between the peers.
	const int MAX_USER_MESSAGE_LENGTH = 4;
	const int MAX_CONNECTION = 10;

	RakNet::RakPeerInterface* rakPeer;
	RakNet::Packet* packet;
	RakNet::RakNetGUID GUID;

	Blockchain* blockchain;
	Block* pendingBlock;
	std::deque<Transaction> pendingTransactions;

	// The C++11 thread for receiving packets in the background.
	std::thread* listenLoopThread;
	// Listens for incoming packets and processes them according to their
	// message identifiers.
	void ListenLoop();
	// The flag for breaking the loop inside the packet listening thread.
	bool isListening;

	

};

