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

	vector<int> vFlow; //���ڼ�����������μ��ȼ�cost


	long long minCost;
	int maxFlow;
	int totalDemand;

	unordered_map<int, int> vToClient;
	vector<long long> vDeployCost; //�������ڸ�����ڵ�Ĳ���ɱ�


	vector<Server *> servers; //�����η�����
	vector<long long> vCost; //�ڵ�ɱ������������ڸ��ڵ���ܷ��ã��������ɱ�+����ɱ���û���������ʱ���ɱ�Ϊ0��
	vector<long long> superEdgeCost; //������cost

	/**
	 * @brief  ����Դ�㣬����ԴΪnetVNum + 1
	 */
	void setSource(int source);

	/**
	* @brief  ���û�㣬������ΪnetVNum
	*/
	void setSink(int sink);

	/**
	 * @brief �����Ƿ��г���Դ 
	 * @param [in] hasSuperSource = true ʱ����ڳ���Դ�����򲻴���      
	 */
	void setSuperSource(bool hasSuperSource);
	
	/**
	 * @brief �����ݷ��������ڽڵ㣩���볬���ߣ�cap = INT_MAX, cost = 0�� 
	 * @param  [in] nodes: ��������������ڵ�
	 */
	void insertSuperEdge(const vector<int> &nodes);


	/**
	* @brief �����ݷ��������ڽڵ㣩���볬����(cap��superCapָ����cost = 0)
	* @param  [in] nodes: ��������������ڵ�
	* @param  [in] superCap: ����������������
	*/
	void insertSuperEdge(const vector<int> &nodes, const vector<long long> &superCap);



	/**
	 * @brief  ����ֻ��һ��Դ����С���ú�������������������Ա����minCost��maxFlow    
	 */
	void calMinCostMaxFlowWithOneSource();

	/**
	* @brief  ������С���ú�������������������Ա����minCost��maxFlow
	*/
	void calMinCostMaxFlow();

	/**
	* @brief  ���㳬����cost��̬�����µ���С���ú�������������������Ա����minCost��maxFlow
	*/
	void calMinCostMaxFlowWithSuperCost();
	

	/**
	* @brief  ���ڴ�ӡ·��
	* @return ·���ַ���
	*/
	string printPaths();


	/**
	* @brief  ���ڴ�ӡ·�����г���Դ��ʹ��
	* @return ·���ַ���
	*/
	vector<int> getServersLocation();

	/**
	 * @brief  ɾ�������ߣ�ÿ�α任������ʱ��ʹ�� 
	 */
	void delSuperEdge();

	/**
	* @brief  ���������
	*/
	void insertDirEdge(int from, int to, int cap, long long cost);

	/**
	* @brief  ���������
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
	
	
	vector<Path *> paths;  //����·��


	void calPaths();

	void calVFlow();
	void calVCost();//����ɱ�+���γɱ�
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