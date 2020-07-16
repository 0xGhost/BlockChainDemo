#pragma once

#include <string>
#include <thread>
#include <iostream>
// Raknet includes
#include "RakPeerInterface.h"
#include "NatPunchthroughClient.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"

using std::string;

class NetworkNode
{
public:
	NetworkNode();
	~NetworkNode();
	
	// Sets maximum number of incoming connections to 1, so that another
		// peer can make a connection request to us.
	void Listen();
	// Attempts to open NAT on the peer with the given GUID. The target peer
	// should be connected to the same NAT punchthrough server as us.
	// 
	// If the NAT punchthrough was successfull, the peer which sent the
	// NAT punchthrough request sends a connection request to the target
	// peer.
	void AttemptNatPunchthrough(string remoteGUID);
	// Sends a string message to the connected peer. The message consists
	// of 3 parts:
	// -Message identifier
	// -Length of the string
	// -An array of characters
	void SendStringMessage(string message);

private:
	// The maximum length for the string messages sent between the peers.
	const int MAX_USER_MESSAGE_LENGTH = 255;
	RakNet::RakPeerInterface* rakPeer;
	RakNet::Packet* packet;
	RakNet::SystemAddress remotePeer;

	// XXX
	// The free of charge NAT punchthrough server of Jenkins Software is
	// hard coded in the constructor.
	RakNet::SystemAddress* natPunchServerAddress;
	RakNet::NatPunchthroughClient natPunchthroughClient;
	// Flag for deciding if the current peer should send a connection
	// request to the remote peer upon a successfull NAT punchthrough.
	bool sentNatPunchthroughRequest;

	// The C++11 thread for receiving packets in the background.
	std::thread* listenLoopThread;
	// Listens for incoming packets and processes them according to their
	// message identifiers.
	void ListenLoop();
	// The flag for breaking the loop inside the packet listening thread.
	bool isListening;


};

