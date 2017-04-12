#ifndef GRAPH_H
#define GRAPH_H


#include <vector>
#include <unordered_map>

using namespace std;

class Graph
{
public:

	Graph(int netVNum, bool hasSuperSource = false);

	struct Edge 
	{
		int from;
		int to;
		int cap;
		long long cost;
		int initCap;
		int initCost;
		Edge *reverseEdge;
		bool isReturnEdge;

		Edge(int from, int to, int cap, long long cost, bool isReturnEdge)
		{
			this->from = from;
			this->to = to;
			this->cap = cap;
			this->cost = cost;

			this->initCap = cap;
			this->initCost = cost;
			this->isReturnEdge = isReturnEdge;
		}

		void setCap(int cap) { this->cap = cap; }
		void setCost(long long cost) { this->cost = cost; }

		void setReverseEdge(Edge *reverseEdge)
		{
			this->reverseEdge = reverseEdge;
		}

		//for net vertex and super sink
		void initEdge()
		{
			cap = initCap;
			cost = initCost;
		}

		
	};

	struct Server
	{
		int cap;
		long long cost;
		Server(int cap, long long cost){ this->cap = cap; this->cost = cost; }
	};

	vector<vector<Edge *>>  adjVec;

	int netVNum; //net vertex number
	int vNum;//graph vertex number

	vector<int> vFlow; //用于计算服务器档次及等价cost


	long long minCost;
	int maxFlow;
	int totalDemand;

	unordered_map<int, int> vToClient;
	vector<long long> vDeployCost; //服务器在各网络节点的部署成本


	vector<Server *> servers; //各档次服务器
	vector<long long> vCost; //节点成本，即服务器在各节点的总费用（服务器成本+部署成本，没部署服务器时，成本为0）
	vector<long long> superEdgeCost; //超级边cost

	/**
	 * @brief  设置源点，超级源为netVNum + 1
	 */
	void setSource(int source);

	/**
	* @brief  设置汇点，超级汇为netVNum
	*/
	void setSink(int sink);

	/**
	 * @brief 设置是否有超级源 
	 * @param [in] hasSuperSource = true 时则存在超级源，否则不存在      
	 */
	void setSuperSource(bool hasSuperSource);
	
	/**
	 * @brief （根据服务器所在节点）插入超级边（cap = INT_MAX, cost = 0） 
	 * @param  [in] nodes: 服务器所在网络节点
	 */
	void insertSuperEdge(const vector<int> &nodes);


	/**
	* @brief （根据服务器所在节点）插入超级边(cap由superCap指定，cost = 0)
	* @param  [in] nodes: 服务器所在网络节点
	* @param  [in] superCap: 服务器最大输出能力
	*/
	void insertSuperEdge(const vector<int> &nodes, const vector<long long> &superCap);



	/**
	 * @brief  计算只有一个源的最小费用和最大流，结果保存至成员变量minCost、maxFlow    
	 */
	void calMinCostMaxFlowWithOneSource();

	/**
	* @brief  计算最小费用和最大流，结果保存至成员变量minCost、maxFlow
	*/
	void calMinCostMaxFlow();

	/**
	* @brief  计算超级边cost动态更新下的最小费用和最大流，结果保存至成员变量minCost、maxFlow
	*/
	void calMinCostMaxFlowWithSuperCost();
	

	/**
	* @brief  用于打印路径
	* @return 路径字符串
	*/
	string printPaths();


	/**
	* @brief  用于打印路径，有超级源下使用
	* @return 路径字符串
	*/
	vector<int> getServersLocation();

	/**
	 * @brief  删除超级边，每次变换超级边时需使用 
	 */
	void delSuperEdge();

	/**
	* @brief  插入有向边
	*/
	void insertDirEdge(int from, int to, int cap, long long cost);

	/**
	* @brief  插入无向边
	*/
	void insertUnDirEdge(int from, int to, int cap, long long cost);
private:
	bool hasSuperSource;

	long long unitFlowCost;

	int source;
	int sink;

	
	vector<bool> known;

	struct Path 
	{
		vector<int> *pathNodes;
		int pathFlow;
		//int pathServerLevel;
		Path(vector<int> *pathNodes, int pathFlow)
		{
			this->pathNodes = pathNodes;
			this->pathFlow = pathFlow;
			//this->pathServerLevel = pathServerLevel;
		}
	};
	
	
	vector<Path *> paths;  //各个路径


	void calPaths();

	void calVFlow();
	void calVCost();//部署成本+档次成本
	int dfs(int source, vector<Edge*> &path);

	int decideServer(int flow);

	bool relabel();
	int augment(int source, int flow);

	string printPath(const Path *path);

	void updateSuperSourceAdj();

	void initNetVertexAdj();

	Edge * insertEdge(int from, int to, int cap, long long cost, bool isReturnEdge = false);


		
};
 






#endif // GRAPH_H