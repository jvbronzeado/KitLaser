#include "solvers/BnB.h"
#include <limits>

Solution BNBSolver::Solve(Data& d)
{
    this->current_data = &d;
    int n = d.getDimension();

    // calcula matriz de custo
	double **cost = new double*[n];
	for (int i = 0; i < n; i++){
		cost[i] = new double[n];
		for (int j = 0; j < n; j++){
		    if(i == j)
		    {
		        cost[i][j] = 1000000000.0; // por algum motivo mais q isso quebra
		        continue;
		    }
			cost[i][j] = d.getDistance(i+1,j+1);
		}
	}

    // pega o AP com o Hungarian Algorithm
    // TODO: Tentar implementar o hungarian algorithm
	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	hungarian_init(&p, cost, n, n, mode); // Carregando o problema
	hungarian_solve(&p);

	hungarian_print_assignment(&p);

	// calcula o subtour inicial
    std::vector<std::vector<int>> subtours = this->GetSubtoursFromAP(&p);

    // printa o subtour calculado
    for(int i = 0; i < subtours.size(); i++)
    {
        for(int j = 0; j < subtours[i].size(); j++)
        {
            std::cout << subtours[i][j] << " ";
        }
        std::cout << std::endl;
    }
	
    // limpa o hungarian algorithm
	hungarian_free(&p);
	for (int i = 0; i < n; i++) delete [] cost[i];
	delete [] cost;
    return Solution();
}

std::vector<std::vector<int>> BNBSolver::GetSubtoursFromAP(hungarian_problem_t* ap)
{
    std::vector<std::vector<int>> output;

    int n = this->current_data->getDimension();
    
    while(true)
    {
        std::vector<int> subtour;

        // acha o ponto de iteração inicial
        int current_row = -1;
        for(int i = 0; i < n; i++)
        {
            for(int j = 0; j < n; j++)
            {
                if(ap->assignment[i][j] == 1)
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
                if(ap->assignment[current_row][j] == 1)
                {
                    success = true;
                    subtour.push_back(current_row + 1);
                    ap->assignment[current_row][j] = 0;
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
            output.push_back(subtour);
        }
        else
        {
            break;
        }
    }
    
    return output;
}
