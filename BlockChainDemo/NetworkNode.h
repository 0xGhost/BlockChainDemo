#pragma once

#include <string>
#include <thread>
#include <iostream>
// Raknet includes
#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

using std::string;

class NetworkNode
{
public:
	NetworkNode(); // using random port
	NetworkNode(const int port);
	~NetworkNode();
	
	void Connect(const int port);

	void SendStringMessage(string message);

private:
	// The maximum length for the string messages sent between the peers.
	const int MAX_USER_MESSAGE_LENGTH = 2048;
	const int MAX_CONNECTION = 10;

	RakNet::RakPeerInterface* rakPeer;
	RakNet::Packet* packet;
	RakNet::SystemAddress remotePeer;



	// The C++11 thread for receiving packets in the background.
	std::thread* listenLoopThread;
	// Listens for incoming packets and processes them according to their
	// message identifiers.
	void ListenLoop();
	// The flag for breaking the loop inside the packet listening thread.
	bool isListening;


};

