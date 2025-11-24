#ifndef SOLVERS_BNB_H_
#define SOLVERS_BNB_H_

#include <limits>

#include "hungarian.h"
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
    std::vector<std::vector<int>> GetSubtoursFromAP(hungarian_problem_t* AP);
    
    Data* current_data;
};

#endif
