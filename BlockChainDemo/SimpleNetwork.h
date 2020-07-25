#pragma once
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::cin;
using std::cout;
using std::endl;
using std::vector;


void addSelfToNetwork(const vector<int>* listOfNodes, int port) {
	printf("Sending port to all nodes\n");
	string message = "#new node#" + port;
	
	for (int i = 0; i < (*listOfNodes).size(); i++) {
		int port = (*listOfNodes)[i];
		cout << "--- sending port to node" << port << endl;

		/*HttpClient client("localhost:" + to_string(port));
		try {
			auto req = client.request("POST", "/addnode", j.dump(3));
			cout << "Node " << port << " Reponse: " << req->content.string() << endl;
		}
		catch (const SimpleWeb::system_error & e) {
			cerr << "Client request error: " << e.what() << endl;
		}*/
	}
}