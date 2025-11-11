#ifndef SOLVERS_ILS_H_
#define SOLVERS_ILS_H_

#include <cassert>

#include "solvers/solver.h"
#include "Data.h"

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
    std::vector<int> GetRandom3Sequence();

    Data* current_data;
};

#endif
