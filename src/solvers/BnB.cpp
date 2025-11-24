#include "solvers/BnB.h"
#include "hungarian.h"
#include <limits>

Solution BNBSolver::Solve(Data& d)
{
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

	double obj_value = hungarian_solve(&p);
	std::cout << "Obj. value: " << obj_value << std::endl;

	std::cout << "Assignment" << std::endl;
	hungarian_print_assignment(&p);

	
    // limpa o hungarian algorithm
	hungarian_free(&p);
	for (int i = 0; i < n; i++) delete [] cost[i];
	delete [] cost;
    return Solution();
}

std::vector<std::vector<int>> GetSubtoursFromAP(std::vector<std::vector<int>> AP)
{
    return AP;    
}
