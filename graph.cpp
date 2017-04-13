#include "graph.h"

#include <limits.h>
#include <string>

#define MIN(a, b) ((a) < (b)) ? (a) : (b)


Graph::Graph(int netVNum, bool hasSuperSource /*= false*/)
{
	this->netVNum = netVNum;
	this->hasSuperSource = hasSuperSource;

	vNum = hasSuperSource ? (netVNum + 2) : (netVNum + 1);
	adjVec.resize(netVNum + 2);
	originAdjVec.resize(netVNum); // zjw
	known.assign(netVNum + 2, false);
	superEdgeCost.resize(netVNum + 2);
	vCost.resize(netVNum);
	vDeployCost.resize(netVNum);
	vFlow.resize(netVNum);

	totalDemand = 0;
}

void Graph::setSuperSource(bool hasSuperSource)
{
	this->hasSuperSource = hasSuperSource;
	vNum = hasSuperSource ? (netVNum + 2) : (netVNum + 1);
}

void Graph::insertSuperEdge(const vector<int> &nodes, const vector<long long> &superCap)
{
	for (int i = 0; i < nodes.size(); ++i)
	{
		insertDirEdge(netVNum + 1, nodes[i], superCap[i], 0);
	}
}

void Graph::insertSuperEdge(const vector<int> &nodes)
{
	for (auto node : nodes)
	{
		insertDirEdge(netVNum + 1, node, INT_MAX, 0);
	}
}

void Graph::setSource(int source)
{
	this->source = source;
}

void Graph::setSink(int sink)
{
	this->sink = sink;
}

void Graph::calMinCostMaxFlowWithOneSource()
{
	minCost = 0;
	maxFlow = 0;
	unitFlowCost = 0;

	initNetVertexAdj();
	do
	{
		known.assign(vNum, false);
		while (augment(source, INT_MAX) > 0)
		{
			known.assign(vNum, false);
		}
	} while (relabel());

	
	minCost += vDeployCost[source] + servers[decideServer(maxFlow)]->cost;
}

void Graph::calMinCostMaxFlow()
{
	minCost = 0;
	maxFlow = 0;
	unitFlowCost = 0;

	initNetVertexAdj();
	do
	{
		known.assign(vNum, false);
		while (augment(source, INT_MAX) > 0)
		{
			known.assign(vNum, false);
		}
	} while (relabel());


	calVCost();
	for (int i = 0; i < netVNum; ++i)
	{
		minCost += vCost[i];
	}
}

void Graph::calMinCostMaxFlowWithSuperCost()
{
	minCost = 0;
	maxFlow = 0;
	unitFlowCost = 0;

	initNetVertexAdj();
	updateSuperSourceAdj();

	do
	{
		known.assign(vNum, false);
		while (augment(source, INT_MAX) > 0)
		{
			known.assign(vNum, false);
		}
	} while (relabel());


	calVCost();
	for (int i = 0; i < netVNum; ++i)
	{
		minCost += vCost[i];
	}
}

// zjw
void Graph::calFlowCostGivenServers(vector<int>& servers, vector<int>& serverTypes) {
	initNetVertexAdj();
	delSuperEdge();
	setSuperEdgesGivenServers(servers, serverTypes);

	minCost = 0;
	maxFlow = 0;
	unitFlowCost = 0;

	do
	{
		known.assign(vNum, false);
		while (augment(source, INT_MAX) > 0)
		{
			known.assign(vNum, false);
		}
	} while (relabel());
}

// zjw
void Graph::setSuperEdgesGivenServers(vector<int>& servers, vector<int>& serverTypes) {
	for (int i = 0; i < servers.size(); i++) {
		int server = servers[i];
		int serverType = serverTypes[i];
		int serverCapacity = this->servers[serverType]->cap;
		insertDirEdge(netVNum + 1, server, serverCapacity, 0);
	}
}

void Graph::calPaths()
{
	paths.clear();
	int tempFlow = 0;
	vector<Edge *> path;
	known.assign(vNum, false);
	while (tempFlow < maxFlow)
	{
		known.assign(vNum, false);
		path.clear();
		tempFlow += dfs(source, path);
	}
}

//计算从超级源流到网络节点的流量
void Graph::calVFlow()
{
	for (int i = 0; i < adjVec[source].size(); ++i)
	{
		vFlow[i] = adjVec[source][i]->reverseEdge->cap;
	}
}

void Graph::calVCost()
{
	calVFlow();

	for (int i = 0; i < netVNum; ++i)
	{
		if (vFlow[i] > 0)
		{
			vCost[i] = vDeployCost[i] + servers[decideServer(vFlow[i])]->cost;
		}
		else
		{
			vCost[i] = 0;
		}
	}
}

int Graph::dfs(int source, vector<Edge*> &path)
{
	if (source == sink)
	{
		int flow = INT_MAX;
		vector<int> *pathNodes = new vector<int>();
		for (auto edge : path)
		{
			flow = MIN((edge->initCap) - (edge->cap), flow);
			if (edge->to != sink)
			{
				pathNodes->push_back(edge->to);
			}
		}
		
		for (auto edge : path)
		{
			edge->cap += flow;
		}
		paths.push_back(new Path(pathNodes, flow));
		return flow;
	}

	known[source] = true;

	for (auto edge : adjVec[source])
	{
		if (edge->isReturnEdge || edge->cap == edge->initCap || known[edge->to])
		{
			continue;
		}
		path.push_back(edge);
		int tempFlow = dfs(edge->to, path);
		if (tempFlow > 0)
		{
			return tempFlow;
		}
		path.pop_back();
	}
	return 0;
}

int Graph::decideServer(int flow)
{
	int i;
	for ( i = 0; i < servers.size(); ++i)
	{
		if (flow <= servers[i]->cap)
		{
			return i;
		}
	}
	return i-1;
}

bool Graph::relabel()
{
	long long tempCost(LLONG_MAX);

	for (int i = 0; i < vNum; ++i)
	{
		if (known[i])
		{
			for (auto edge : adjVec[i])
			{
				if (edge->cap > 0 && !known[edge->to] && edge->cost < tempCost)
				{
					tempCost = edge->cost;
				}
			}
		}
	}

	if (tempCost == LLONG_MAX)
	{
		return false;
	}

	for (int i = 0; i < vNum; ++i)
	{
		if (known[i])
		{
			for (auto edge : adjVec[i])
			{
				edge->cost -= tempCost;
				edge->reverseEdge->cost += tempCost;
			}
		}
	}

	unitFlowCost += tempCost;
	return true;
}

int Graph::augment(int source, int flow)
{
	if (source == sink)
	{
		minCost += unitFlowCost * flow;
		maxFlow += flow;
		return flow;
	}

	known[source] = true;

	int leftFlow = flow;
	int tempFlow;
	for (auto edge : adjVec[source])
	{
		if (edge->cap > 0 && edge->cost == 0 && !known[edge->to])
		{
			tempFlow = augment(edge->to, MIN(edge->cap, leftFlow));

			edge->cap -= tempFlow;
			edge->reverseEdge->cap += tempFlow;
			leftFlow -= tempFlow;
			if (leftFlow <= 0)
			{
				return flow;
			}
		}
	}
	return flow - leftFlow;
}

std::string Graph::printPath(const Path *path)
{
	string result = "";
	for (auto e : *(path->pathNodes))
	{
		result += to_string(e) + " ";
	}
	result += to_string(vToClient[(*(path->pathNodes))[(*(path->pathNodes)).size() - 1]]) + " " + to_string(path->pathFlow) 
		    + " "+to_string(decideServer(vFlow[(*(path->pathNodes))[0]]));
	return result;
}

std::string Graph::printPaths()
{
	calVFlow();//根据流量选择服务器档次

	calPaths();

	string result = to_string(paths.size()) + "\n";
	for (auto path : paths)
	{
		result += "\n" + printPath(path);
	}
	return result;
}

//需在initNetVertexAdj()后使用
void Graph::updateSuperSourceAdj()
{
	if (hasSuperSource)
	{
		for (int i = 0; i < adjVec[source].size(); ++i)
		{
			adjVec[source][i]->cap = INT_MAX;
			adjVec[source][i]->cost = superEdgeCost[i];
			adjVec[source][i]->reverseEdge->cap = 0;
			adjVec[source][i]->reverseEdge->cost = -superEdgeCost[i];
		}
	}
}

void Graph::initNetVertexAdj()
{
	//没考虑选定服务器位置情况
	for (int i = 0; i <= sink; ++i)
	{
		for (int j = 0; j < adjVec[i].size(); ++j)
		{
			adjVec[i][j]->initEdge();
		}
	}
}

Graph::Edge * Graph::insertEdge(int from, int to, int cap, long long cost, bool isReturnEdge /*= false*/)
{
	Edge * edge = new Edge(from, to, cap, cost, isReturnEdge);
	adjVec[from].push_back(edge);

	// insert edge to originAdjVec, zjw.
	if (!edge->isReturnEdge && edge->from < netVNum) {
		originAdjVec[from].push_back(edge);
	}

	return edge;
}

void Graph::insertDirEdge(int from, int to, int cap, long long cost)
{
	Edge * edge = insertEdge(from, to, cap, cost);
	Edge * returnEdge = insertEdge(to, from, 0, -cost, true);
	edge->reverseEdge = returnEdge;
	returnEdge->reverseEdge = edge;
}

void Graph::insertUnDirEdge(int from, int to, int cap, long long cost)
{
	insertDirEdge(from, to, cap, cost); 
	insertDirEdge(to, from, cap, cost);
}

vector<int> Graph::getServersLocation()
{
	vector<int> serversLocation;
	for (auto edge : adjVec[source])
	{
		if (edge->cap < INT_MAX)
		{
			serversLocation.push_back(edge->to);
		}
	}
	return serversLocation;
}

void Graph::delSuperEdge()
{
	for (int i = 0; i < adjVec[netVNum + 1].size(); ++i)
	{
		delete adjVec[netVNum + 1][i]->reverseEdge;
		adjVec[adjVec[netVNum + 1][i]->to].pop_back();
	}
	adjVec[netVNum + 1].clear();
}

