#include "solvers/BnB.h"
#include "solvers/ILS.h"
#include <limits>

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

	BNBNode test_node;
	this->UpdateNode(test_node);

    // limpa a matriz de custo
	for (int i = 0; i < n; i++) delete [] cost[i];
	delete[] this->cost;
    return Solution();
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
	hungarian_solve(&p);
    
	// calcula o subtour inicial e seu menor subtour
    this->GetSubtoursFromAP(node, p);

    // printa o subtour calculado
    for(int i = 0; i < node.subtour.size(); i++)
    {
        for(int j = 0; j < node.subtour[i].size(); j++)
        {
            std::cout << node.subtour[i][j] << " ";
        }
        std::cout << std::endl;
    }
    
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
    node.feasible = node.subtour.size() == 1;
}
