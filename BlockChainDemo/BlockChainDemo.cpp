// Yiang Lu 03/05/2020

#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <thread> 
#include <mutex>


// include openssl
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <assert.h>

// include RakNet
#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "RakNetTypes.h"
#ifdef _WIN32
#include "Kbhit.h"
#include "WindowsIncludes.h" // Sleep
#else
#include "Kbhit.h"
#include <unistd.h> // usleep
#endif
#include "Gets.h"

#if LIBCAT_SECURITY==1
#include "SecureHandshake.h" // Include header for secure handshake
#endif

//#include "MSTimeStamp.h"
#include "NetworkNode.h"
#include "Blockchain.h"
#include "RSATool.h"
#include "Player.h"

//using namespace std;
using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::thread;
using std::ifstream;
using std::ofstream;
using std::unordered_map;

string privateKey = "-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEowIBAAKCAQEAy8Dbv8prpJ/0kKhlGeJYozo2t60EG8L0561g13R29LvMR5hy\n"\
"vGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+vw1HocOAZtWK0z3r26uA8kQYOKX9\n"\
"Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQApfc9jB9nTzphOgM4JiEYvlV8FLhg9\n"\
"yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68i6T4nNq7NWC+UNVjQHxNQMQMzU6l\n"\
"WCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoVPpY72+eVthKzpMeyHkBn7ciumk5q\n"\
"gLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUywQIDAQABAoIBADhg1u1Mv1hAAlX8\n"\
"omz1Gn2f4AAW2aos2cM5UDCNw1SYmj+9SRIkaxjRsE/C4o9sw1oxrg1/z6kajV0e\n"\
"N/t008FdlVKHXAIYWF93JMoVvIpMmT8jft6AN/y3NMpivgt2inmmEJZYNioFJKZG\n"\
"X+/vKYvsVISZm2fw8NfnKvAQK55yu+GRWBZGOeS9K+LbYvOwcrjKhHz66m4bedKd\n"\
"gVAix6NE5iwmjNXktSQlJMCjbtdNXg/xo1/G4kG2p/MO1HLcKfe1N5FgBiXj3Qjl\n"\
"vgvjJZkh1as2KTgaPOBqZaP03738VnYg23ISyvfT/teArVGtxrmFP7939EvJFKpF\n"\
"1wTxuDkCgYEA7t0DR37zt+dEJy+5vm7zSmN97VenwQJFWMiulkHGa0yU3lLasxxu\n"\
"m0oUtndIjenIvSx6t3Y+agK2F3EPbb0AZ5wZ1p1IXs4vktgeQwSSBdqcM8LZFDvZ\n"\
"uPboQnJoRdIkd62XnP5ekIEIBAfOp8v2wFpSfE7nNH2u4CpAXNSF9HsCgYEA2l8D\n"\
"JrDE5m9Kkn+J4l+AdGfeBL1igPF3DnuPoV67BpgiaAgI4h25UJzXiDKKoa706S0D\n"\
"4XB74zOLX11MaGPMIdhlG+SgeQfNoC5lE4ZWXNyESJH1SVgRGT9nBC2vtL6bxCVV\n"\
"WBkTeC5D6c/QXcai6yw6OYyNNdp0uznKURe1xvMCgYBVYYcEjWqMuAvyferFGV+5\n"\
"nWqr5gM+yJMFM2bEqupD/HHSLoeiMm2O8KIKvwSeRYzNohKTdZ7FwgZYxr8fGMoG\n"\
"PxQ1VK9DxCvZL4tRpVaU5Rmknud9hg9DQG6xIbgIDR+f79sb8QjYWmcFGc1SyWOA\n"\
"SkjlykZ2yt4xnqi3BfiD9QKBgGqLgRYXmXp1QoVIBRaWUi55nzHg1XbkWZqPXvz1\n"\
"I3uMLv1jLjJlHk3euKqTPmC05HoApKwSHeA0/gOBmg404xyAYJTDcCidTg6hlF96\n"\
"ZBja3xApZuxqM62F6dV4FQqzFX0WWhWp5n301N33r0qR6FumMKJzmVJ1TA8tmzEF\n"\
"yINRAoGBAJqioYs8rK6eXzA8ywYLjqTLu/yQSLBn/4ta36K8DyCoLNlNxSuox+A5\n"\
"w6z2vEfRVQDq4Hm4vBzjdi3QfYLNkTiTqLcvgWZ+eX44ogXtdTDO7c+GeMKWz4XX\n"\
"uJSUVL5+CVjKLjZEJ6Qc2WZLl94xSwL71E41H4YciVnSCQxVc4Jw\n"\
"-----END RSA PRIVATE KEY-----\n\0";

string publicKey = "-----BEGIN PUBLIC KEY-----\n"\
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAy8Dbv8prpJ/0kKhlGeJY\n"\
"ozo2t60EG8L0561g13R29LvMR5hyvGZlGJpmn65+A4xHXInJYiPuKzrKUnApeLZ+\n"\
"vw1HocOAZtWK0z3r26uA8kQYOKX9Qt/DbCdvsF9wF8gRK0ptx9M6R13NvBxvVQAp\n"\
"fc9jB9nTzphOgM4JiEYvlV8FLhg9yZovMYd6Wwf3aoXK891VQxTr/kQYoq1Yp+68\n"\
"i6T4nNq7NWC+UNVjQHxNQMQMzU6lWCX8zyg3yH88OAQkUXIXKfQ+NkvYQ1cxaMoV\n"\
"PpY72+eVthKzpMeyHkBn7ciumk5qgLTEJAfWZpe4f4eFZj/Rc8Y8Jj2IS5kVPjUy\n"\
"wQIDAQAB\n"\
"-----END PUBLIC KEY-----\n";

extern const int NOZ = 4; // num of zero for the hash
extern const int NOT = 20; // num of transactions in a block

#define NOM 5 // num of miner
#define NOB 8 // num of new Block pending

//#define LOCAL_SIMULATION 

#ifdef LOCAL_SIMULATION
Blockchain myBlockChain(NOZ);
int newBlockAdded = 0; // the newest block message boardcast through miners  
thread miners[NOM];

std::mutex mtx;

//void MiningFinish(const int id, Block* newBlock, const Blockchain& blockchain, Block** newBlocks);

void minerFunc(const int id, const Blockchain& blockchain, Block** newBlocks)
{
	while (newBlockAdded < NOB)
	{
		// get copy from newBlocks array
		Block* newBlock = new Block(*newBlocks[newBlockAdded]);
		newBlock->SetPrevBlock(*blockchain.GetChain().back());
		cout << "miner" << id << " start mining block" << newBlockAdded << endl;


		int temp = newBlockAdded;
		if (newBlock->ThreadingStoppableMine(NOZ, newBlockAdded))
		{
			
			//std::lock_guard<std::mutex> lockGuard(mutex);
			if (newBlockAdded == temp)
			{
				cout << "new block" << newBlockAdded << "  mined. Miner: " << id << " nonce:" << newBlock->GetNonce() <<  endl;
				++newBlockAdded;
				myBlockChain.AddBlock(newBlock);
			}
		}
		else
		{
			int a = 1;
		}
	}
	cout << "myBlockchain mining finish." << endl;
}

void CheaterFunc(Blockchain* blockchain, Block** newBlocks)
{
	int newBlockCount = 0;
	while (newBlockCount < NOB)
	{
		// get copy from newBlocks array
		Block* newBlock = new Block(*newBlocks[newBlockCount]);
		newBlock->SetPrevBlock(*(blockchain->GetChain().back()));
		cout << "Cheater start mining block" << newBlockCount << endl;

		newBlock->Mine(NOZ, true);


		cout << "Cheater blockchain new block" << newBlockCount << "  mined." << " nonce:" << newBlock->GetNonce() << endl;

		blockchain->AddBlock(newBlock);
		newBlockCount++;
	}
	cout << "cheater mining finished." << endl;
}

//void MiningFinish(const int id, Block* newBlock, const Blockchain& blockchain, Block** newBlocks)
//{
//	cout << "Mining new Block" << newBlockAdded <<" finish. Miner: " + id << endl;
//
//	myBlockChain.AddBlock(newBlock);
//	newBlockAdded++;
//	if (newBlockAdded > NOB)
//	{
//		cout << "All new block mined for myblockchain." << endl;
//		return;
//	}
//
//	for (int i = 0; i < NOM; i++)
//	{
//		miners[i] = thread(minerFunc, i + 1, blockchain, newBlocks);
//	}
//}

#endif

unsigned char GetPacketIdentifier(RakNet::Packet* p)
{
	if (p == 0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)p->data[0];
}

int main()
{
	//string s = TimeStamp().ToString();
	//cout << s << endl;

	//std::tm t;
	//std::istringstream ss(s);
	//int i;
	//ss >> std::get_time(&t, "%a/%b/%d/%Y/%T.") >>i;
	//long long time;
	//time = mktime(&t);
	//std::chrono::milliseconds ms(i);
	//std::ostringstream os;
	//std::cout << std::put_time(std::localtime(&time), "%a/%b/%d/%Y/%T") <<"." <<ms.count()<<  std::endl;

	//return  0;
#ifndef LOCAL_SIMULATION

	bool isMiner = true;

	Blockchain* blockchain;
	vector<Player*> players;

	Player alice("Alice");
	Player bob("Bob");
	Player john("John");

	players.push_back(&alice);
	players.push_back(&bob);
	players.push_back(&john);

	Transaction t1(&alice, &bob, "Sword");
	alice.Sign(t1);
	Transaction t2(&bob, &alice, "Gem");
	bob.Sign(t2);
	Transaction t3(&bob, &john, "Sword");
	bob.Sign(t3);

	Block b1(1);
	b1.AddTransaction(t1, true);
	b1.AddTransaction(t2, true);
	b1.AddTransaction(t3, true);
	b1.AddTransaction(t3, true);
	b1.AddTransaction(t2, true); // 5
	b1.AddTransaction(t1, true); 
	b1.AddTransaction(t1, true);
	b1.AddTransaction(t2, true);
	b1.AddTransaction(t3, true);
	b1.AddTransaction(t3, true); // 10
	b1.AddTransaction(t2, true);
	b1.AddTransaction(t1, true);
	b1.AddTransaction(t1, true);
	b1.AddTransaction(t2, true);
	b1.AddTransaction(t3, true); // 15
	b1.AddTransaction(t3, true);
	b1.AddTransaction(t2, true);
	b1.AddTransaction(t1, true);
	b1.AddTransaction(t1, true);
	b1.AddTransaction(t2, true); // 20


	Block b2(2);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true); // 5
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true); // 10
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true); // 15
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true);
	b2.AddTransaction(t2, true);
	b2.AddTransaction(t3, true); // 20

	Block b3(3);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true); // 5
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true); // 10
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true); // 15
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true);
	b3.AddTransaction(t1, true);
	b3.AddTransaction(t2, true); // 20

	int port;
	cout << "Enter port you want to listen on: ";
	cin >> port;

	NetworkNode node(port);

	cout << "Enter \"\\c\" to connect other node\n"
		<< "Enter \"\\a\" to mine and boardcast new block with 20 random transactions\n"
		<< "Enter \"\\s\" to boardcast blockchain\n"
		<< "Enter \"\\i\" to initialize blockchain\n"
		<< "Enter \"\\b\" to view current blockchain\n"
		<< "Enter \"\\p\" to view current pending transaction\n"
		<< "Enter \"\\r\" to request latest blockchain\n"
		<< "Enter \"\\t\" to boardcast random transactions (simulate user)\n";
		//<< "Enter \"\\m\" to switch between user and miner(default) mode\n";


	while (1)
	{
		if (kbhit())
		{
			string input;
			cin >> input;
			if (input == "\\c")
			{
				cout << "Enter port you want to connect: (0 to skip)";
				cin >> port;
				if (port != 0)
				{
					node.Connect(port);
				}
			}
			else if (input == "\\a")
			{
				Block* b = new Block(node.GetBlockchain()->GetChain().size());
				for (int i = 0; i < NOT; i++)
				{
					int tID = rand() % 3;
					Transaction* t = &t1;
					std::stringstream ss;
					switch (tID)
					{
					case 0: t = &t1; break;
					case 1: t = &t2; break;
					case 2: t = &t3; break;
					}
					b->AddTransaction(*t, true);

					//b->AddTransaction(*t);
				}
				cout << "Mining new block......" << endl;
				node.GetBlockchain()->AddBlock(b);
				cout << *(node.GetBlockchain()->GetChain().back())
					<< "Block verify: " << node.GetBlockchain()->GetChain().back()->Verify(NOZ) << endl << endl;
				node.SendBlock(*b);

			}
			else if (input == "\\s")
			{
				node.SendBlockchain();
			}
			else if (input == "\\i")
			{
#pragma region Initial blockchain
				cout << "How many block you want to add into blockchain(0~3)?  \n";
				int num = 0;
				cin >> num;
				cout << "Initializing blockchain..." << endl;
				blockchain = new Blockchain(NOZ);

				if (num >= 1)
				{
					cout << "Mining block 1..." << endl;
					blockchain->AddBlock(&b1);
					cout << *(blockchain->GetChain().back())
						<< "Block verify: " << blockchain->GetChain().back()->Verify(NOZ) << endl << endl;
				}
				if (num >= 2)
				{
					cout << "Mining block 2..." << endl;
					blockchain->AddBlock(&b2);
					cout << *(blockchain->GetChain().back())
						<< "Block verify: " << blockchain->GetChain().back()->Verify(NOZ) << endl << endl;
				}
				if (num >= 3)
				{
					cout << "Mining block 3..." << endl;
					blockchain->AddBlock(&b3);
					cout << *(blockchain->GetChain().back())
						<< "Block verify: " << blockchain->GetChain().back()->Verify(NOZ) << endl << endl;
				}

				node.SetBlockchain(blockchain);
#pragma endregion

			}
			else if (input == "\\b")
			{
				cout << *(node.GetBlockchain()) << endl;
			}
			else if (input == "\\p")
			{
				if (node.GetPendingTransactions().size() > 0)
				{
					for (Transaction t : node.GetPendingTransactions())
						cout << t << endl;
				}
				else
					cout << "No pending transcation" << endl;
			}
			else if (input == "\\r")
			{
				node.SendRequestForLatestBlockchain();
			}

			else if (input == "\\t")
			{
				cout << "Enter how many transactions you want to boardcast: ";
				int num;
				cin >> num;
				for (int i = 0; i < num; i++)
				{
					int tID = rand() % 3;
					Transaction* t = &t1;
					switch (tID)
					{
					case 0: t = &t1; break;
					case 1: t = &t2; break;
					case 2: t = &t3; break;
					}
					t->ResetTimeStamp();
					node.CollectNewTransaction(*t);
					node.SendTransaction(*t);
					//cout << *t << endl;
				}
			}
			//else if (input == "\\m")
			//{
			//	isMiner = !isMiner;
			//	cout << "now is in " << (isMiner ? "miner" : "user") << " mode" << endl;
			//}
			else
				node.SendStringMessage(input);
		}
		node.PendingBlockCheck();
	}

	return 0;

#else // local simlutation

	vector<Player*> players;

	Player alice("Alice");
	Player bob("Bob");
	Player john("John");

	players.push_back(&alice);
	players.push_back(&bob);
	players.push_back(&john);

	Transaction t1(&alice, &bob, "Sword");
	alice.Sign(t1);
	Transaction t2(&bob, &alice, "Gem");
	bob.Sign(t2);
	Transaction t3(&bob, &john, "Sword");
	bob.Sign(t3);



	string input;

	cout << "initial blockchain demo? y/n: ";
	cin >> input;

	if (input == "y" || input == "Y")
	{

		cout << "Creating the chain and mining block 0..." << endl;
		Blockchain blockChain(NOZ);
		cout << *(blockChain.GetChain().back()) << endl;

		cout << "Mining block 1..." << endl;
		Block b1(1);
		b1.AddTransaction(t1);
		b1.AddTransaction(t2);
		b1.AddTransaction(t3);
		blockChain.AddBlock(&b1);
		cout << *(blockChain.GetChain().back())
			<< "Block verify: " << blockChain.GetChain().back()->Verify(NOZ) << endl << endl;

		cout << "Mining block 2..." << endl;
		Block b2(2);
		b2.AddTransaction(t2);
		b2.AddTransaction(t3);
		blockChain.AddBlock(&b2);
		cout << *(blockChain.GetChain().back())
			<< "Block verify: " << blockChain.GetChain().back()->Verify(NOZ) << endl << endl;


		cout << "Mining block 3..." << endl;
		Block b3(3);
		b3.AddTransaction(t1);
		b3.AddTransaction(t2);
		blockChain.AddBlock(&b3);
		cout << *(blockChain.GetChain().back())
			<< "Block verify: " << blockChain.GetChain().back()->Verify(NOZ) << endl << endl;

		cout << "Saving copies of blockchain to players\n";

		ofstream outputFile;
		for (const Player* player : players)
		{
			outputFile.open(player->GetName() + "_copy.txt");
			if (outputFile.is_open())
			{
				for (const Block* b : blockChain.GetChain())
					outputFile << *b << endl;
			}
			else
			{
				cout << player->GetName() + " file save failed\n";
			}
			outputFile.close();
		}
	}

	cout << "/---------------------- distributed demo begin --------------------------/" << endl;
	vector<Blockchain> blockchains;
	ifstream inputFile;
	for (const Player* player : players)
	{
		inputFile.open(player->GetName() + "_copy.txt");
		if (inputFile.is_open())
		{
			cout << "\n\n            /****** Reading blockchain copy from " + player->GetName() + " file *******/" << endl;
			Blockchain bc(NOZ);
			inputFile >> bc;
			inputFile >> std::ws;
			cout << bc;
			for (Block* b : bc.GetChain())
			{
				cout << "Verify block" << b->GetIndex() << ": " << b->Verify(NOZ) << endl;
				if (!b->Verify(NOZ))
				{
					cout << "Remining..." << endl;
					b->Mine(NOZ);
					cout << *b;
				}
			}


			blockchains.emplace_back(bc);
		}
		else
		{
			cout << player->GetName() + " file read failed\n";
		}


		inputFile.close();
	}

	cout << "\nAlice last hash: " << blockchains[0].GetChain().back()->GetHash() << endl;
	cout << "Bob last hash: " << blockchains[1].GetChain().back()->GetHash() << endl;
	cout << "John last hash: " << blockchains[2].GetChain().back()->GetHash() << endl;

	cout << "/****** start voting ******/" << endl;
	

	unordered_map<string, int> lastHashCount;
	int maxCount = 0;
	string maxCountLastHash;

	for (Blockchain bc : blockchains)
	{
		string lastHash = bc.GetChain().back()->GetHash();
		auto search = lastHashCount.find(lastHash);
		if (search != lastHashCount.end())
		{
			lastHashCount[lastHash]++;
			if (maxCount < lastHashCount[lastHash])
			{
				maxCountLastHash = lastHash;
				maxCount = lastHashCount[lastHash];
				myBlockChain = bc;
			}
		}
		else
		{
			lastHashCount[lastHash] = 1;
		}
	}

	cout << "voting result: " << maxCountLastHash << endl;



	cout << "(not implement yet)correct the user file that give the wrong vote? y/n: ";
	cin >> input;

	if (input == "y" || input == "Y")
	{
		// TODO: write the correct copy to the wrong user's file
	}

	cout << "/****** add new block to blockchain and cheater add fake block ******/" << endl;
	int index = myBlockChain.GetChain().back()->GetIndex() + 1;

	Blockchain cheaterBlockChain = myBlockChain;

	Block realBlock(index);
	realBlock.AddTransaction(t1);
	realBlock.AddTransaction(t2);
	realBlock.AddTransaction(t3);

	Block fakeBlock(index);
	fakeBlock.AddTransaction(t1);
	fakeBlock.AddTransaction(t3);

	cout << "cheater mining fake block into cheater blockchain" << endl;
	cheaterBlockChain.AddBlock(&fakeBlock);

	cout << "mining realBlock into my blockchain" << endl;
	myBlockChain.AddBlock(&realBlock);

	cout << "Add 8 new blocks into blockchain. 5 miner(miner1 to miner5) VS 1 cheater(miner0)" << endl;

	Block* newBlocks[NOB];
	for (int i = 0; i < NOB; i++)
	{
		newBlocks[i] = new Block(++index);
		newBlocks[i]->AddTransaction(t1);
		newBlocks[i]->AddTransaction(t2);
	}

	for (int i = 0; i < NOM; i++)
	{
		miners[i] = thread(minerFunc, (i + 1), myBlockChain, newBlocks);
	}
	std::thread cheater(CheaterFunc, &cheaterBlockChain, newBlocks);


	//std::thread cheater(CheaterFunc, &cheaterBlockChain, newBlocks);

	/*for (int i = 0; i < NOB; i++)
	{	
		Block* newBlock = new Block(*newBlocks[i]);
		cout << "cheater start mining new Block" << i << endl;
		cheaterBlockChain.AddBlock(newBlock);
		
	}
	cout << "cheater mining finished." << endl;*/
	
	for (int i = 0; i < NOM; i++)
	{
		miners[i].join();
	}
	cheater.join();
#endif
	return 0;
}
