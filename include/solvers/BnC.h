#ifndef SOLVERS_BNC_H_
#define SOLVERS_BNC_H_

#include <list>
#include <queue>

#include "hungarian.h"
#include "Data.h"
#include "solvers/solver.h"
#include "solvers/ILS.h"

#include <ilcplex/ilocplex.h>

struct SeparationResult {
    std::vector<int> alpha;
    double beta=-1;
};

class BNCSolver : public Solver
{
public:
    BNCSolver() = default;
    ~BNCSolver() = default;

    Solution Solve(Data& d);
private:
    
    Data* current_data;
    double** cost;
};

#endif
