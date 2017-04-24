#include "search.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <unordered_map>
#include <random>
#include <limits.h>

#define RAND(a, b) ((a)+(rand())%((b)-(a)+1))

void Search::search() {
	initialize();
	sortServerAndType(bestServers, bestServerTypes);
	unordered_map<int, int> serverIndex;
	for (int i = 0; i < bestServers.size(); i++) {
		serverIndex.insert({bestServers[i], i});
	}
	bestCost = drop(bestServers, bestServerTypes);
	cout << bestCost << endl;
	for (int server : bestServers) {
		cout << server << " ";
	}
	cout << "\n" << endl;
	firstDrop = false;
	//reintroduceDroppedServers(droppedServers, serverIndex);
	// cout << "reintroduce" << ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 << "ms" << endl;
	if (graph->netVNum < 800) {
	addDropped();
	sortServerAndType(bestServers, bestServerTypes);
	}
	add();
}

void Search::addDropped() {
	sortServers(droppedServers);
	//reverse(droppedServers.begin(), droppedServers.end());
	for (int i = 0; i < droppedServers.size(); i++) {
		if (((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 > 70000) {
			break;
		}
		int server = droppedServers[i];
		bestServers.push_back(server);
		bestServerTypes.push_back(initialServerTypes[server]);
		int newCost = modifyServerType(bestServers, bestServerTypes);
		if (newCost < bestCost) {
			bestCost = newCost;
			cout << "add: " << bestCost << endl;
		}
		else {
			bestServers.pop_back();
			bestServerTypes.pop_back();
		}
	}
}

void Search::add() {
	vector<int> allVertice = getAllVertices();
	sortServers(allVertice);
	addServerAscent(allVertice);
}

void Search::addServerAscent(vector<int>& candidates) {
	vector<int> tempServers = bestServers;
	vector<int> tempServerTypes = bestServerTypes;
	int tempCost = bestCost;
	
	int index = 0;
	int nonImprove = 0;
	while (index < candidates.size() && ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 < 88000) {
		int newServer = candidates[index];
		index++;
		if (find(tempServers.begin(), tempServers.end(), newServer) != tempServers.end()) {
			continue;
		}
		tempServers.push_back(newServer);
		tempServerTypes.push_back(initialServerTypes[newServer]);

		tempCost = getAllCost(tempServers, tempServerTypes);
		if (tempCost > bestCost) {
			tempCost = drop(tempServers, tempServerTypes);
			tempCost = modifyServerType(tempServers, tempServerTypes);
		}

		// ----- get actual output----- //
		/*cout << "actual output: ";
		for (int server : bestServers) {
		cout << graph->adjVec[server].back()->cap << " ";
		}
		cout << endl;*/
		// ----- end -----//

		if (tempCost < bestCost) {
			bestServers = tempServers;
			bestCost = tempCost;
			bestServerTypes = tempServerTypes;
			cout << "new best servers location by adding neighbor " << newServer << "\n";
			cout << "initial type " << initialServerTypes[newServer] << "\n";
			cout << ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 << "ms\n";
			vector<int> intialTypes;
			for (auto server : bestServers) {
				cout << server << " ";
				intialTypes.push_back(initialServerTypes[server]);
			}
			cout << "\n";
			for (auto server : intialTypes) {
				cout << server << " ";
			}
			cout << "\n";
			for (auto server : bestServerTypes) {
				cout << server << " ";
			}
			cout << "new best cost: " << bestCost << endl;
			//index = 0;
			nonImprove = 0;
		}
		else {
			tempServers = bestServers;
			tempCost = bestCost;
			tempServerTypes = bestServerTypes;
			nonImprove++;
		}
		if (nonImprove >= 10) {
			preBreak = false;
			index = 0;
			startIndex = 0;
		}
	}
}

int Search::drop(vector<int>& servers, vector<int>& serverTypes) {
	int dropIndex = startIndex;
	int newCost;
	int bestCost = getAllCost(servers, serverTypes);
	//if (!firstDrop) {
	//	sortServerAndType(servers, serverTypes);
	//	reverse(servers.begin(), servers.end());
	//	reverse(serverTypes.begin(), serverTypes.end());
	//	//sortByActualOutput(servers, serverTypes);
	//}
	bool changeStart = false;
	while (dropIndex < servers.size() && ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 < 88000) {
		int droppedServer = servers[dropIndex];
		if (!firstDrop && (graph->adjVec[droppedServer].back()->cap > graph->servers[serverTypes.back()]->cap || !isDroppable(droppedServer))) {
			dropIndex++;
			continue;
		}
		int droppedServerType = serverTypes[dropIndex];
		servers.erase(servers.begin() + dropIndex);
		serverTypes.erase(serverTypes.begin() + dropIndex);

		newCost = getAllCost(servers, serverTypes);
		if (newCost <= bestCost) {
			bestCost = newCost;
			if (firstDrop) {
				droppedServers.push_back(droppedServer);
			}
			else {
				if (!changeStart) {
					startIndex = dropIndex;
					changeStart = true;
				}
				if (preBreak && graph->netVNum > 800) {
					break;
				}
			}
		}
		else {
			servers.insert(servers.begin() + dropIndex, droppedServer);
			serverTypes.insert(serverTypes.begin() + dropIndex, droppedServerType);
			dropIndex++;
		}
	}
	return bestCost;
}

bool Search::isDroppable(int server) {
	int leftCap = 0;
	for (int i = 0; i < bestServers.size(); i++) {
		if (bestServers[i] != server) {
			leftCap += graph->servers[bestServerTypes[i]]->cap - graph->adjVec[bestServers[i]].back()->cap;
		}
	}
	return leftCap > graph->adjVec[server].back()->cap;
}

void Search::reintroduceDroppedServers(vector<int>& droppedServers, unordered_map<int, int> serverIndex) {
	sortServers(droppedServers);

	for (int i = 0; i < droppedServers.size(); i++) {
		if (((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 > 50000) {
			break;
		}
		int count = 0;
		int server = droppedServers[i];
		int index = serverIndex[server];
		int newCost;
		for (int j = 0; j < bestServers.size(); j++) {
			if (count >= 20 || ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 > 50000) {
				break;
			}
			if (serverIndex[bestServers[j]] < index) {
				continue;
			}
			count++;
			int changedServer = bestServers[i];
			vector<int> tempServerType = bestServerTypes;
			bestServers[i] = server;
			bestServerTypes[i] = initialServerTypes[server];
			newCost = modifyServerType(bestServers, bestServerTypes);
			if (newCost < bestCost) {
				droppedServers[i] = changedServer;
				bestCost = newCost;
				cout << "move: " << bestCost << endl;
				break;
			}
			else {
				bestServers[i] = changedServer;
				bestServerTypes = tempServerType;
			}
		}
	}
}

int Search::modifyServerType(vector<int>& servers, vector<int>& serverTypes, int cost) {
	if (cost == 0) {
		cost = getAllCost(servers, serverTypes);
	}
	if (cost == INT_MAX) {
		return INT_MAX;
	}
	for (int i = 0; i < servers.size(); i++) {
		int serverType = serverTypes[i];
		int server = servers[i];
		int usedBandwidth = graph->adjVec[server].back()->cap;
		for (int j = 0; j < typeCount; j++) {
			if (usedBandwidth == 0) {
				break;
			}
			if (j >= serverType) {
				break;
			}
			if (graph->servers[j]->cap >= usedBandwidth) {
				serverTypes[i] = j;
				cost -= graph->servers[serverType]->cost - graph->servers[j]->cost;
				break;
			}
		}
	}
	return cost;
}

void Search::initialize() {
	initializeNodes();
	initializeServerTypes();

	bestServers.reserve(graph->clientNum);
	for (int i = 0; i < graph->clientNum; i++) {
		bestServers.push_back(graph->clientVertexId[i]);
		// bestServerTypes.push_back(typeCount - 1);
		bestServerTypes.push_back(initialServerTypes[graph->clientVertexId[i]]);
	}
}

void Search::initializeNodes() {
	nodes.reserve(graph->netVNum);
	for (int i = 0; i < graph->netVNum; i++) {
		// initialize nodes.
		int demand = 0;
		if (graph->vToClient.find(i) != graph->vToClient.end()) {
			demand = graph->clientDemand[graph->vToClient[i]];
		}
		nodes.push_back(new Node(i, demand, graph->vDeployCost[i]));
	}
}

void Search::initializeServerTypes() {
	 /*int potentialOutput;
	 for (int i = 0; i < graph->netVNum; i++) {
	 	potentialOutput = calPotentialOutput(nodes[i]);
	 	for (int j = 0; j < typeCount; j++) {
	 		if (graph->servers[j]->cap >= potentialOutput || j == typeCount - 1) {
	 			initialServerTypes.push_back(j);
	 			break;
	 		}
	 	}
	 }*/
	
	for (int i = 0; i < graph->netVNum; i++) {
		int bestType = -1;
		double bestPerCost = 100000;
		int depolyCost = graph->vDeployCost[i];
		for (int j = 0; j < typeCount; j++) {
			double perCost = (double)(depolyCost + graph->servers[j]->cost) / graph->servers[j]->cap * 1.0;
			if (perCost < bestPerCost || (perCost == bestPerCost && j > bestType)) {
				bestPerCost = perCost;
				bestType = j;
				continue;
			}
		}
		initialServerTypes.push_back(bestType);
	}

}

int Search::getAllCost(vector<int>& servers, vector<int>& serverTypes) {
	 //auto start = clock();
	graph->calFlowCostGivenServers(servers, serverTypes);
	 //auto end = clock();
	 //cout << "time" << " : " << (((float)end - start) / CLOCKS_PER_SEC) * 1000.0 << "ms\n";
	if (graph->maxFlow < graph->totalDemand) {
		return INT_MAX;
	}

	vector<int> usedServerIndex;
	for (int i = 0; i < servers.size(); i++) {
		int server = servers[i];
		if (graph->adjVec[server].back()->cap > 0) {
			usedServerIndex.push_back(i);
		}
	}

	for (int index : usedServerIndex) {
		graph->minCost += graph->servers[serverTypes[index]]->cost;
		graph->minCost += graph->vDeployCost[servers[index]];
	}

	return graph->minCost;
}

vector<int> Search::getAllVertices() {
	vector<int> allVertice;
	allVertice.reserve(graph->netVNum);
	for (int i = 0; i < graph->netVNum; i++) {
		allVertice.push_back(i);
	}
	return allVertice;
}

int Search::calPotentialOutput(Node * node) {
	int output = 0;
	for (auto e : graph->originAdjVec[node->vertexId]) {
		output += e->initCap;
	}
	return output + node->demands;
}

void Search::sortServers(vector<int>& servers, bool byDemands) {
	vector<Node*> sortHelper;
	for (int server : servers) {
		sortHelper.push_back(nodes[server]);
	}
	if (byDemands) {
		sort(sortHelper.begin(), sortHelper.end(), sortByDemandsComparator);
	}
	else {
		sort(sortHelper.begin(), sortHelper.end(), [this](Node* o1, Node* o2) { return sortByOutputComparator(o1, o2); });
	}
	for (int i = 0; i < sortHelper.size(); i++) {
		servers[i] = sortHelper[i]->vertexId;
	}
}

void Search::sortServerAndType(vector<int>& servers, vector<int>& serverTypes) {
	unordered_map<int, int> serverToType;
	for (int i = 0; i < servers.size(); i++) {
		serverToType.insert({servers[i], serverTypes[i]});
	}
	sortServers(bestServers, true);
	 /*sortServers(bestServers);
	 reverse(bestServers.begin(), bestServers.end());*/
	for (int i = 0; i < servers.size(); i++) {
		serverTypes[i] = serverToType[servers[i]];
	}
}

void Search::sortByActualOutput(vector<int>& servers, vector<int>& serverTypes) {
	unordered_map<int, int> serverToType;
	for (int i = 0; i < servers.size(); i++) {
		serverToType.insert({servers[i], serverTypes[i]});
	}
	sort(servers.begin(), servers.end(), [this](int o1, int o2) { return graph->adjVec[o1].back()->cap < graph->adjVec[o2].back()->cap; });
	for (int i = 0; i < servers.size(); i++) {
		serverTypes[i] = serverToType[servers[i]];
	}
}