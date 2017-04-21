#include "deploy.h"
#include <stdio.h>
#include <ctime>
#include <iostream>
#include "graph.h"
#include "search.h"

//你要完成的功能总入口



using namespace std;
void deploy_server(char * topo[MAX_EDGE_NUM], int line_num, char * filename)
{
	int netVNum, linkNum, clientVNum;
	sscanf(topo[0], "%d %d %d", &netVNum, &linkNum, &clientVNum);

	Graph cdn(netVNum);
	cdn.clientNum = clientVNum; // zjw 
	cdn.clientVertexId.reserve(clientVNum); // zjw

	int serLevelId, serCap, serCost;
	
	int i;
	for (i = 2; sscanf(topo[i], "%d %d %d", &serLevelId, &serCap, &serCost) == 3; ++i)
	{
		Graph::Server *server = new Graph::Server(serCap, ((long long)serCost));
		cdn.servers.push_back(server);
	}
	
	int netVId, deployCost;
	
	for (i += 1; sscanf(topo[i], "%d %d", &netVId, &deployCost) == 2; ++i)
	{
		cdn.vDeployCost[netVId] = (long long)deployCost;
	}

	int from, to, cap, cost;
	
	for (i += 1; sscanf(topo[i], "%d %d %d %d", &from, &to, &cap, &cost) == 4; ++i)
	{
		cdn.insertUnDirEdge(from, to, cap, (long long)cost);
	}

	int clientId, clientDemand;

	cdn.clientDemand.reserve(clientVNum); // zjw
	for (i += 1; i < line_num ; ++i)
	{
		sscanf(topo[i], "%d %d %d", &clientId, &netVId, &clientDemand);
		cdn.vToClient.insert(pair<int, int>(netVId, clientId));
		cdn.clientVertexId.push_back(netVId); // zjw
		cdn.clientDemand.push_back(clientDemand); // zjw
		cdn.insertDirEdge(netVId, netVNum, clientDemand, 0);
		cdn.totalDemand += clientDemand;
	}

	//----------end of initialize------------//

	cdn.setSink(netVNum);
	cdn.setSuperSource(true);
	cdn.setSource(netVNum + 1);

	Search search(&cdn);
	auto begin = clock();
	search.search();
	
	for (auto server : search.bestServerTypes) {
		cout << server << " ";
	}
	cout << "\n" << endl;

	auto end = clock();
	cout << "time"  << " : " << (((float)end - begin) / CLOCKS_PER_SEC * 1000.0) << "ms\n";
	

	int bestCost = search.modifyServerType(search.bestServers, search.bestServerTypes);

	cdn.bestServers = search.bestServers;
	cdn.bestServerTypes = search.bestServerTypes;
	cout << "modify: " << bestCost << endl;

	for (auto server : search.bestServerTypes) {
		cout << server << " ";
	}
	cout << "\n" << endl;

	cout << "maxFlow  " << cdn.maxFlow << "  ALL Demand: " << cdn.totalDemand << endl;

	// for (auto server : search.bestServerTypes) {
	// 	cout << server << " ";
	// }
	// cout << "\n" << endl;
	// cout << bestCost << endl;
	 //char * topo_file = (char *) cdn.printPaths().data();
	
	 //write_result(topo_file, filename);

}
