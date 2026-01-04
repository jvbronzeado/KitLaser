#include "solvers/RL.h"
#include "Kruskal.h"
#include "solvers/ILS.h"
#include <algorithm>
#include <limits>

constexpr int MAX_DISTANCE = 99999999;

// pode ser 0 para DFS, 1 para BFS, e 2 para minimo LB
#define BRANCHING_STRATEGY 0

Solution RLSolver::Solve(Data& d)
{
    this->current_data = &d;
    const int n = d.getDimension();

    // usa o ILS para resolver o TSP de forma heuristica
    Solution heuristico = ILSSolver().Solve(d);
    
    // inicializa a matriz de custo q vai ser utilizada pra o hungarian    
	this->cost = std::vector<std::vector<double>>(n, std::vector<double>(n));

    // cria o node da raiz da tree
	RLNode root;
	root.penalizador.resize(n, 0);
	
	// cria a arvore
	#if BRANCHING_STRATEGY != 2
    std::list<RLNode> tree;
    tree.push_back(root);
    #else
    std::priority_queue<RLNode> tree;
    tree.push(root);
	#endif

	double upper_bound = heuristico.cost;

    // algoritmo
	while(!tree.empty())
	{
	    #if BRANCHING_STRATEGY == 0
	    RLNode node = tree.back();
	    tree.pop_back();
	    #elif BRANCHING_STRATEGY == 1
        RLNode node = tree.front();
        tree.pop_front();
        #else
        RLNode node = tree.top();
        tree.pop();
        #endif

        TreeResult result = this->GetTree(node, upper_bound);
        if(node.lower_bound > upper_bound)
        {
            continue;
        }

        if(!result.tree->treeHasNonTwoEdge())
        {
            std::vector<double> p(n, 0);
            upper_bound = std::min(upper_bound, result.tree->getCost(p));
        }
        else
        {
            // pega maior grau
            const std::vector<int>& degrees = result.tree->getDegrees();
            std::vector<int>::const_iterator it = std::max_element(degrees.begin(), degrees.end());
            int index = static_cast<int>(it - degrees.begin());
            
            // itera entre as nodes e adiciona os forbidden nodes
            const vii& edges = result.tree->getEdges();
            for(int i = 0; i < edges.size(); i++) {
                auto& [a, b] = edges[i];
                
                if(a == index || b == index) {
                    // checa se ja foi inserido nos forbiddens
                    int u = a == index ? b : a;

                    bool already = false;
                    for (auto& [x, y] : node.forbidden) {
                        if (x == u && y == index) {
                            already = true;
                            break;
                        }
                    }

                    if (already) continue; // se ja foi então não precisa adicionar essa node, seria uma duplicada

                    RLNode new_node = node;
                    new_node.forbidden.push_back(make_pair(u, index));

                    #if BRANCHING_STRATEGY != 2
                    tree.push_back(new_node);
                    #else
                    tree.push(new_node);      
                    #endif
                }
            }
        }
	}
    
	Solution optimal;
	optimal.cost = upper_bound;
    return optimal;
}

TreeResult RLSolver::DualLagrangian(double UB, const std::vector<double>& def_pen) {
    constexpr double emin = 1e-5;
    constexpr int kmax = 30;

    const int n = this->current_data->getDimension();
    
    std::vector<double> penalizador = def_pen;
    std::vector<double> penalizador_diff(n, 0);
    double e = 1;
    int k = 0;

    TreeResult result = {nullptr, {}};
    while(e > emin && (result.tree == nullptr || result.tree->treeHasNonTwoEdge())) {
        std::unique_ptr<Kruskal> s = this->MSTAlgorithm(penalizador);
        double w = s->getCost(penalizador);

        double total_sum = 0;
        for(int i = 1; i < n; i++) {
            const int degree = s->getDegrees()[i];
            penalizador_diff[i] = (2 - degree);
            total_sum += penalizador_diff[i] * penalizador_diff[i];
        }
        
        if(result.tree == nullptr || w > result.tree->getCost(result.penalizador)) {
            result.tree = std::move(s);
            result.penalizador = penalizador;
            k = 0;
        }
        else {
            k++;
            if(k > kmax) {
                k = 0;
                e /= 2;
            }
        }

        if(total_sum == 0)
            break;
        
        double u = e * ((UB - w) / total_sum);
        for(int i = 1; i < n; i++) {
            penalizador[i] += penalizador_diff[i] * u;
        }
    }

    return result;
}

std::unique_ptr<Kruskal> RLSolver::MSTAlgorithm(const std::vector<double>& penalizador) {
    const int n = this->current_data->getDimension();

    std::vector<std::vector<double>> pen_cost = cost;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++) {
            if(pen_cost[i][j] != MAX_DISTANCE) {
                pen_cost[i][j] -= penalizador[i] + penalizador[j];
            }
        }
    }

	std::unique_ptr<Kruskal> k = make_unique<Kruskal>(pen_cost);
	double cost = k->OneTree(n, pen_cost);

    return k;
}

TreeResult RLSolver::GetTree(RLNode& node, double UB)
{
    int n = this->current_data->getDimension();

    // calcula matriz de custo
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
		    if(i == j)
		    {
		        cost[i][j] = MAX_DISTANCE; // por algum motivo mais q isso quebra
		        continue;
		    }
			cost[i][j] = this->current_data->getDistance(i+1,j+1);
		}
	}

	// aplica os forbidden no custo
	for(auto& [a, b] : node.forbidden)
	{
	    cost[a][b] = MAX_DISTANCE;
    }

    TreeResult tree = this->DualLagrangian(UB, node.penalizador);
    node.penalizador = tree.penalizador;
    node.lower_bound = tree.tree->getCost(node.penalizador);

    return tree;
}
