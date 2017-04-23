#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include "graph.h"

using namespace std;

// zjw
class Search
{
public:
	vector<int> bestServers;
	vector<int> bestServerTypes;

	Search(Graph *graph) { 
		this->graph = graph; 
		typeCount = graph->servers.size();
	}
	Graph *graph;
	void search();
	int modifyServerType(vector<int>& servers, vector<int>& serverTypes, int cost = 0);
	int getAllCost(vector<int>& servers, vector<int>& serverTypes);

	struct Node
	{
		int vertexId;
		int demands;
		int cost;

		Node(int vertex, int demands, int cost) {
			this->vertexId = vertex;
			this->demands = demands;
			this->cost = cost;
		}	
	};

private:
	int typeCount;
	int bestCost;
	
	vector<int> initialServerTypes;
	vector<Node*> nodes;
	vector<int> droppedServers;
	bool firstDrop = true;
	bool preBreak = true;
	void initialize();
	void initializeNodes();
	void initializeServerTypes();
	void add();
	void addDropped();
	void addServerAscent(vector<int>& candidates);
	void sortByActualOutput(vector<int>& servers, vector<int>& serverTypes);
	int drop(vector<int>& servers, vector<int>& serverTypes);
	void reintroduceDroppedServers(vector<int>& droppedServers, unordered_map<int, int> serverIndex);
	int calPotentialOutput(Node*);
	vector<int> getAllVertices();
	bool isDroppable(int server);
	void sortServers(vector<int>& servers, bool byDemands = false);
	void sortServerAndType(vector<int>& servers, vector<int>& serverTypes);
	static bool sortByDemandsComparator(Node* o1, Node* o2) {
		return (o1->demands < o2->demands);
	}
	bool sortByOutputComparator(Node* o1, Node* o2) {
		return (double)calPotentialOutput(o1) / graph->vDeployCost[o1->vertexId] * 1.0 >(double) calPotentialOutput(o2) / graph->vDeployCost[o2->vertexId]*1.0;
		//return calPotentialOutput(o1) > calPotentialOutput(o2);
	}
};


#endif