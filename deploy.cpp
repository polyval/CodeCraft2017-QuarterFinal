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
	cdn.setSink(netVNum);

	//int allFlow = 0;
	//for (int i = 0; i < netVNum; ++i)
	//{
	//	cdn.setSource(i);

	//	auto begin = clock();
	//	cdn.calMinCostMaxFlowWithOutSuperSource();
	//	auto end = clock();
	//	cout << "time" << i << " : " << end - begin << "ms\n";
	//	allFlow += cdn.maxFlow;
	//	cout << i << "  "<<cdn.maxFlow <<endl;
	//}

	

	cdn.setSuperSource(true);
	cdn.setSource(netVNum + 1);

	Search search(&cdn);
	auto begin = clock();
	search.search();
	auto end = clock();
	cout << "time"  << " : " << end - begin << "ms\n";
		
	cout << "maxFlow  " << cdn.maxFlow << "  ALL Demand: " << cdn.totalDemand << endl;
/*
		cout << cdn.printPaths().data();*/
	//cout << "ALL flow: " << flow<< " ALL Demand: " << cdn.totalDemand;
	//cin.get();
//	 需要输出的内容
//////	char * topo_file = (char *)"17\n\n0 8 0 20\n21 8 0 20\n9 11 1 13\n21 22 2 20\n23 22 2 8\n1 3 3 11\n24 3 3 17\n27 3 3 26\n24 3 3 10\n18 17 4 11\n1 19 5 26\n1 16 6 15\n15 13 7 13\n4 5 8 18\n2 25 9 15\n0 7 10 10\n23 24 11 23";
//
//
//
//	// 直接调用输出文件的方法输出到指定文件中(ps请注意格式的正确性，如果有解，第一行只有一个数据；第二行为空；第三行开始才是具体的数据，数据之间用一个空格分隔开)
//	//write_result(topo_file, filename);

}
