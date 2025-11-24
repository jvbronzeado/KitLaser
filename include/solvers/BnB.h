#ifndef SOLVERS_BNB_H_
#define SOLVERS_BNB_H_

#include <limits>

#include "Data.h"
#include "solvers/solver.h"
#include "solvers/ILS.h"

class BNBSolver : public Solver
{
public:
    BNBSolver() = default;
    ~BNBSolver() = default;

    Solution Solve(Data& d);
private:
    std::vector<std::vector<int>> HungarianAlgorithm(Data& d);
    std::vector<std::vector<int>> GetSubtoursFromAP(std::vector<std::vector<int>> AP);
    
    Data* current_data;
};

#endif
