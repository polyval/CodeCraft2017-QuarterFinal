#include "search.h"
#include <algorithm>
#include <ctime>
#include <iostream>
#include <limits.h>


void Search::search() {
	initialize();
	sortServers(bestServers, true);
	add();
	modifyServerType();
}


void Search::add() {
	vector<int> allVertice;
	allVertice.reserve(graph->netVNum);
	for (int i = 0; i < graph->netVNum; i++) {
		allVertice.push_back(i);
	}
	sortServers(allVertice);
	addServerAscent(allVertice);
}

void Search::addServerAscent(vector<int>& candidates) {
	vector<int> tempServers = bestServers;
	vector<int> tempServerTypes = bestServerTypes;
	int tempCost = bestCost;
	
	int index = 0;
	while (index < candidates.size() && ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 < 88500) {
		int newServer = candidates[index];
		index++;
		if (find(bestServers.begin(), bestServers.end(), newServer) != bestServers.end()) {
			continue;
		}

		bestServers.push_back(newServer);
		bestServerTypes.push_back(initialServerTypes[newServer]);
		bestCost = getAllCost(bestServers, bestServerTypes);
		drop();
		modifyServerType();
		if (bestCost < tempCost) {
			tempServers = bestServers;
			tempCost = bestCost;
			tempServerTypes = bestServerTypes;
			cout << "new best servers location by adding neighbor " << newServer << "\n";
			cout << "new best cost: " << bestCost << endl;
			index = 0;
		}
		else {
			bestServers = tempServers;
			bestCost = tempCost;
			bestServerTypes = tempServerTypes;
		}
	}
}

void Search::drop() {
	int dropIndex = 0;
	int newCost;
	while (dropIndex < bestServers.size() && ((float)clock() - graph->startTime) / CLOCKS_PER_SEC * 1000.0 < 88500) {
		int droppedServer = bestServers[dropIndex];
		if (graph->adjVec[droppedServer].back()->cap > graph->servers[bestServerTypes.back()]->cap) {
			dropIndex++;
			continue;
		}
		int droppedServerType = bestServerTypes[dropIndex];
		bestServers.erase(bestServers.begin() + dropIndex);
		bestServerTypes.erase(bestServerTypes.begin() + dropIndex);

		newCost = getAllCost(bestServers, bestServerTypes);
		if (newCost < bestCost) {
			bestCost = newCost;
		}
		else {
			bestServers.insert(bestServers.begin() + dropIndex, droppedServer);
			bestServerTypes.insert(bestServerTypes.begin() + dropIndex, droppedServerType);
			dropIndex++;
		}
	}
}

void Search::modifyServerType() {
	graph->calFlowCostGivenServers(bestServers, bestServerTypes);
	for (int i = 0; i < bestServers.size(); i++) {
		int serverType = bestServerTypes[i];
		int server = bestServers[i];
		int usedBandwidth = graph->adjVec[server].back()->cap;
		for (int j = 0; j < typeCount; j++) {
			if (j >= serverType) {
				break;
			}
			if (graph->servers[j]->cap >= usedBandwidth) {
				bestServerTypes[i] = j;
				bestCost -= graph->servers[serverType]->cost - graph->servers[j]->cost;
				break;
			}
		}
	}
}

void Search::initialize() {
	initializeNodes();
	initializeServerTypes();

	bestServers.reserve(graph->clientNum);
	for (int i = 0; i < graph->clientNum; i++) {
		bestServers.push_back(graph->clientVertexId[i]);
		bestServerTypes.push_back(typeCount - 1);
		//bestCost += graph->servers.back()->cost;
		//bestCost += graph->vDeployCost[graph->clientVertexId[i]];
	}
	bestCost = getAllCost(bestServers, bestServerTypes);
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
	int potentialOutput;
	for (int i = 0; i < graph->netVNum; i++) {
		potentialOutput = calPotentialOutput(nodes[i]);
		for (int j = 0; j < typeCount; j++) {
			if (graph->servers[j]->cap >= potentialOutput || j == typeCount - 1) {
				initialServerTypes.push_back(j);
				break;
			}
		}
	}
}

int Search::getAllCost(vector<int>& servers, vector<int>& serverTypes) {
	// auto start = clock();
	graph->calFlowCostGivenServers(servers, serverTypes);
	// auto end = clock();
	// cout << "time" << " : " << (((float)end - start) / CLOCKS_PER_SEC) * 1000.0 << "ms\n";
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
		graph->minCost += graph->vDeployCost[index];
	}
	return graph->minCost;
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
