#ifndef SOLVERS_ILS_H_
#define SOLVERS_ILS_H_

#include <cassert>
#include <vector>
#include <algorithm>

#include "solvers/solver.h"
#include "Data.h"

struct ILSInsertionInfo
{
    int inserted_node;
    int removed_edge;
    double cost;
};

class ILSSolver : public Solver
{
public:
    ILSSolver();
    ~ILSSolver();

    Solution Solve(Data& d);
private:
    // algorithm functions
    Solution Construcao();
    Solution Pertubacao(Solution& s);
    void BuscaLocal(Solution& s);

    // helper functions
    void GenerateRandom3Sequence();
    void GenerateInsertionCosts(Solution& s, size_t starting_i);

    Data* current_data;

    // solve members
    std::vector<int> cl;
    std::vector<int> si;
    std::vector<ILSInsertionInfo> insertion_costs;
};

#endif
