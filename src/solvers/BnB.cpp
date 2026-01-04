#include "solvers/BnB.h"
#include "solvers/ILS.h"
#include <algorithm>
#include <limits>

// pode ser 0 para DFS, 1 para BFS, e 2 para minimo LB
#define BRANCHING_STRATEGY 2

Solution BNBSolver::Solve(Data& d)
{
    this->current_data = &d;
    int n = d.getDimension();

    // usa o ILS para resolver o TSP de forma heuristica
    Solution heuristico = ILSSolver().Solve(d);
    
    // cria a matriz de custo q vai ser utilizada pra o hungarian    
	this->cost = new double*[n];
	for (int i = 0; i < n; i++){
		cost[i] = new double[n];
	}

    // cria o node da raiz da tree
	BNBNode root;
	this->UpdateNode(root);

	// cria a arvore
	#if BRANCHING_STRATEGY != 2
    std::list<BNBNode> tree;
    tree.push_back(root);
    #else
    std::priority_queue<BNBNode> tree;
    tree.push(root);
	#endif

	double upper_bound = heuristico.cost + 1;

    // algoritmo
	while(!tree.empty())
	{
	    #if BRANCHING_STRATEGY == 0
	    BNBNode node = tree.back();
	    tree.pop_back();
	    #elif BRANCHING_STRATEGY == 1
        BNBNode node = tree.front();
        tree.pop_front();
        #else
        BNBNode node = tree.top();
        tree.pop();
        #endif

        if(node.lower_bound > upper_bound)
        {
            continue;
        }

        if(node.feasible)
        {
            upper_bound = std::min(upper_bound, node.lower_bound);
        }
        else
        {
            for(int i = 0; i < node.subtour[node.chosen].size() - 1; i++)
            {
                BNBNode n;
                n.forbidden_arcs = node.forbidden_arcs;

                std::pair<int,int> arc = {
                    node.subtour[node.chosen][i],
                    node.subtour[node.chosen][i + 1]
                };

                n.forbidden_arcs.push_back(arc);
                this->UpdateNode(n);

                if(n.lower_bound <= upper_bound)
                {
                    #if BRANCHING_STRATEGY != 2
                    tree.push_back(n);
                    #else
                    tree.push(n);      
                    #endif
                }
            }
        }
	}

    // limpa a matriz de custo
	for (int i = 0; i < n; i++) delete [] cost[i];
	delete[] this->cost;

	Solution optimal;
	optimal.cost = upper_bound;
    return optimal;
}

void BNBSolver::UpdateNode(BNBNode& node)
{
    int n = this->current_data->getDimension();

    // calcula matriz de custo
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
		    if(i == j)
		    {
		        cost[i][j] = 99999999; // por algum motivo mais q isso quebra
		        continue;
		    }
			cost[i][j] = this->current_data->getDistance(i+1,j+1);
		}
	}

	// aplica os forbidden no custo
	for(auto& pair : node.forbidden_arcs)
	{
	    cost[pair.first-1][pair.second-1] = 99999999;
    }

    // pega o AP com o Hungarian Algorithm
    // TODO: Tentar implementar o hungarian algorithm
	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, cost, n, n, mode); // Carregando o problema
	node.lower_bound = hungarian_solve(&p);
    
	// calcula o subtour inicial e seu menor subtour
    this->GetSubtoursFromAP(node, p);
    
    // limpa o hungarian algorithm
	hungarian_free(&p);
}

void BNBSolver::GetSubtoursFromAP(BNBNode& node, hungarian_problem_t& ap)
{
    node.subtour.clear();
    int n = this->current_data->getDimension();

    int smallest_subtour_index = 0;
    int smallest_subtour_size = std::numeric_limits<int>::max();

    while(true)
    {
        std::vector<int> subtour;

        // acha o ponto de iteração inicial
        int current_row = -1;
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                if(ap.assignment[i][j] == 1)
                {
                    current_row = i;
                    break;
                }
            }

            if(current_row != -1)
                break;
        }

        while(current_row != -1) // itera entre os assignments até o subtour repetir
        {
            bool success = false; // se for verdadeiro é pq achou um proximo ponto para ir
            // procura se existe um ponto para ir na linha atual
            for(int j = 0; j < n; j++)
            {
                if(ap.assignment[current_row][j] == 1)
                {
                    success = true;
                    subtour.push_back(current_row + 1);
                    ap.assignment[current_row][j] = 0;
                    current_row = j; // vai para o proximo ponto
                    break;
                }
            }

            if(not success)
            {
                subtour.push_back(current_row + 1);                
                break;
            }
        }

        if(subtour.size() != 0)
        {
            if(subtour.size() < smallest_subtour_size)
            {
                smallest_subtour_index = node.subtour.size();
                smallest_subtour_size = subtour.size();
            }
            
            node.subtour.push_back(subtour);
        }
        else
        {
            break;
        }
    }

    node.chosen = smallest_subtour_index;
    node.feasible = (node.subtour.size() == 1) && (node.subtour[0][0] == node.subtour[0][node.subtour[0].size()-1] && node.subtour[0].size() == (n + 1));
}
