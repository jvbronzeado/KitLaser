#ifndef SOLVERS_ILS_H_
#define SOLVERS_ILS_H_

#include "solvers/solver.h"
#include "Data.h"

#define MAX_ITER 3
#define MAX_ITER_ILS 3

class ILS : public Solver
{
public:
    ILS();
    ~ILS();

    Solution Solve(Data& d);
private:
    Solution Construcao();
    Solution Pertubacao(Solution& s);
    void BuscaLocal(Solution& s);
};

#endif
