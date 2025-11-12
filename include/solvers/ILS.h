#ifndef SOLVERS_ILS_H_
#define SOLVERS_ILS_H_

#include <cassert>
#include <vector>
#include <algorithm>
#include <chrono>
#include <stdint.h>

#include "solvers/solver.h"
#include "Data.h"

struct ILSInsertionInfo
{
    int node_index;
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
    void GenerateInsertionCosts(Solution& s);

    // neighbourhood structures
    bool BestImprovementSwap(Solution& s);
    bool BestImprovementOPTOPT(Solution& s);
    bool BestImprovementOrOpt(Solution& s, uint8_t len);

    Data* current_data;

    // solve members
    std::vector<int> cl;
    std::vector<int> si;
    std::vector<ILSInsertionInfo> insertion_costs;
};

#endif
