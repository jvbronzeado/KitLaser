#ifndef SOLVERS_RL_H_
#define SOLVERS_RL_H_

#include <list>
#include <queue>

#include "hungarian.h"
#include "Data.h"
#include "solvers/solver.h"
#include "solvers/ILS.h"

struct BNBNode
{
    std::vector<std::pair<int, int>> forbidden_arcs; // lista de arcos proibidos do nó
    std::vector<std::vector<int>> subtour; // conjunto de subtours da solução
    double lower_bound; // custo total da solução do algoritmo hungaro
    int chosen; // indice de menor subtour
    bool feasible; // indica se a solucao AP_TSP é viavel
    
    bool operator<(const BNBNode& other) const
    {
        return this->lower_bound > other.lower_bound; // para o priority_queue
    }
};

class BNBSolver : public Solver
{
public:
    BNBSolver() = default;
    ~BNBSolver() = default;

    Solution Solve(Data& d);
private:
    std::vector<std::vector<int>> HungarianAlgorithm(Data& d);

    void UpdateNode(BNBNode& node);
    void GetSubtoursFromAP(BNBNode& node, hungarian_problem_t& AP);
    
    Data* current_data;
    double** cost;
};

#endif
