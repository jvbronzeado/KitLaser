#ifndef SOLVERS_MLP_H_
#define SOLVERS_MLP_H_

#include <cassert>
#include <vector>
#include <algorithm>
#include <chrono>
#include <stdint.h>

#include "solvers/solver.h"
#include "Data.h"


struct MLPSubsequence
{
    double T, C;
    int W;
    int first, last;
    MLPSubsequence Concatenate(Data* d, MLPSubsequence& sigma_2);
};

class MLPSolver : public Solver
{
public:
    MLPSolver();
    ~MLPSolver();

    Solution Solve(Data& d);
private:
    // algorithm functions
    Solution Construcao();
    Solution Pertubacao(Solution& s);
    void BuscaLocal(Solution& s);

    // helper functions
    void UpdateAllSubseq(Solution& s);
    void UpdateSubseqRange(Solution& s, int begin, int end);

    // neighbourhood structures
    bool BestImprovementSwap(Solution& s);
    bool BestImprovementOPTOPT(Solution& s);
    bool BestImprovementOrOpt(Solution& s, uint8_t len);

    Data* current_data;

    // solve members
    std::vector<int> cl;
    std::vector<std::vector<MLPSubsequence>> subseq_matrix;
    double mlp_total_cost = 0;
};

#endif
