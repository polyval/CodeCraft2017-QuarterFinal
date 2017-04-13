#ifndef SEARCH_H
#define SEARCH_H

#include <vector>
#include "graph.h"

using namespace std;

// zjw
class Search
{
public:
	Search(Graph *graph) { 
		this->graph = graph; 
		typeCount = graph->servers.size();
	}
	Graph *graph;
	void search();

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
	vector<int> bestServers;
	vector<int> bestServerTypes;
	vector<int> initialServerTypes;
	vector<Node*> nodes;

	void initialize();
	void initializeNodes();
	void initializeServerTypes();
	void add();
	void addServerAscent(vector<int>& candidates);
	void drop();
	void modifyServerType();
	int getAllCost(vector<int>& servers, vector<int>& serverTypes);
	int calPotentialOutput(Node*);
	void sortServers(vector<int>& servers, bool byDemands = false);
	static bool sortByDemandsComparator(Node* o1, Node* o2) {
		return (o1->demands < o2->demands);
	}
	bool sortByOutputComparator(Node* o1, Node* o2) {
		return calPotentialOutput(o1) > calPotentialOutput(o2);
	}
};












#endif