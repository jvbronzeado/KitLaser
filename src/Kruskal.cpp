#include "Kruskal.h"
#include <limits>

Kruskal::Kruskal(const vvi& dist){
	for(int i = 1; i < dist.size(); ++i){
		for(int j = 1; j < dist[i].size(); ++j){
			graph.push( make_pair(-dist[i][j], make_pair(i, j)) );
		}	
	}
}

void Kruskal::initDisjoint(int n){
	pset.resize(n);
	degrees.resize(n);
	for (int i = 0; i < n; ++i){
		pset[i] = i;
		degrees[i] = 0;
	}
}

int Kruskal::findSet(int i){
	return (pset[i] == i) ? i : (pset[i] = findSet(pset[i]));
}

void Kruskal::unionSet(int i, int j){
	pset[findSet(i)] = findSet(j);
}

bool Kruskal::isSameSet(int i, int j){
	return (findSet(i) == findSet(j))? true:false;
}

vii Kruskal::getEdges(){
	return edges;
}

const std::vector<int>& Kruskal::getDegrees() {
	return this->degrees;
}

double Kruskal::getCost(const std::vector<double>& penalizadores) {
	double c = this->cost;
	for(int i = 0; i < penalizadores.size(); i++) {
		c += 2 * penalizadores[i];
	}
	return c;
}

bool Kruskal::treeHasNonTwoEdge() {
	for(int i = 0; i < degrees.size(); i++) {
		if(degrees[i] != 2) {
			return true;
		}
	}

	return false;
}

double Kruskal::MST(int nodes){
	initDisjoint(nodes);

	double cost = 0;
	
	while(!graph.empty()){
		pair<double, ii> p = graph.top();
		graph.pop();

		if(!isSameSet(p.second.first, p.second.second)){
			edges.push_back(make_pair(p.second.first, p.second.second));
			degrees[p.second.first]++;
			degrees[p.second.second]++; 
			cost += (-p.first);
			unionSet(p.second.first, p.second.second);
		}
	}

	this->cost = cost;
	return cost;
}

double Kruskal::OneTree(int nodes, const vvi& dist) {
	double cost = this->MST(nodes);

	std::array<double, 2> smallest = {std::numeric_limits<double>().max()};
	std::array<int, 2> indexes = {0};

	for(int i = 1; i < nodes; i++) {
		for(int j = 0; j < 2; j++) {
			if(dist[0][i] < smallest[j]) {
				if(j == 0) {
					smallest[1] = smallest[0];
					indexes[1] = indexes[0];
				}

				smallest[j] = dist[0][i];
				indexes[j] = i;
				break;
			}
		}
	}

	cost += (smallest[0] + smallest[1]);
	edges.push_back(make_pair(0, indexes[0]));
	edges.push_back(make_pair(0, indexes[1]));
	degrees[0] += 2;
	degrees[indexes[0]]++;
	degrees[indexes[1]]++;
 
	this->cost = cost;
	return cost;
}
