#ifndef KRUSKAL_H
#define KRUSKAL_H

#include <cstdio>
#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <utility>

using namespace std;

typedef pair<int, int> ii;
typedef vector<vector<double>> vvi;
typedef vector<ii> vii;

class Kruskal{
public:
	Kruskal(const vvi& dist);

	double MST(int nodes);
	double OneTree(int nodes, const vvi& dist); // NOTA: essa função já chama o MST
	vii getEdges();

	const std::vector<int>& getDegrees();
	double getCost(const std::vector<double>& penalizadores);

	bool treeHasNonTwoEdge();
private:
	priority_queue<pair<double,ii> > graph;
	vector<int> pset;
	vector<int> degrees;
	vii edges;
	double cost;
	
	void initDisjoint(int n);
	int findSet(int i);
	void unionSet(int i, int j);
	bool isSameSet(int i, int j);
};

#endif
