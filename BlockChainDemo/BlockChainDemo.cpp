// Yiang Lu 03/05/2020

#include <iostream>
#include <fstream>
#include <unordered_map>

#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <assert.h>

#include "Blockchain.h"
#include "RSATool.h"
#include "Player.h"

using namespace std;

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

#define NOZ 4 // num of zero for the hash





int main()
{
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
			}
		}
		else
		{
			lastHashCount[lastHash] = 1;
		}
	}

	cout << "voting result: "  << maxCountLastHash << endl;

	cout << "(not implement yet)correct the user file that give the wrong vote? y/n: ";
	cin >> input;

	if (input == "y" || input == "Y")
	{
		// TODO: write the correct copy to the wrong user's file
	}

	cout << "/****** add new block ******/" << endl;



	return 0;
}
