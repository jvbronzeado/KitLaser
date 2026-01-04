#ifndef SOLVERS_BNB_H_
#define SOLVERS_BNB_H_

#include <list>
#include <queue>

#include "hungarian.h"
#include "Data.h"
#include "solvers/solver.h"
#include "solvers/ILS.h"
#include "solvers/BnB.h"
#include "Kruskal.h"

struct RLNode {
    std::vector<std::pair<int, int>> forbidden;
    std::vector<double> penalizador;
    double lower_bound;

    bool operator<(const RLNode& other) const
    {
        return this->lower_bound > other.lower_bound; // para o priority_queue
    }
};

struct TreeResult {
    std::unique_ptr<Kruskal> tree;
    std::vector<double> penalizador;
};

class RLSolver : public Solver
{
public:
    RLSolver() = default;
    ~RLSolver() = default;

    Solution Solve(Data& d);
private:
    TreeResult DualLagrangian(double UB, const std::vector<double>& def_penalizador);    
    std::unique_ptr<Kruskal> MSTAlgorithm(const std::vector<double>& penalizador);

    TreeResult GetTree(RLNode& node, double UB);

    double SolveLagrangianDual();
    
    Data* current_data;
    std::vector<std::vector<double>> cost;
};

#endif
