#include "../../include/solvers/solver.h"

void Solution::Print()
{
    for(int i = 0; i < this->sequence.size()-1; i++)
        std::cout << this->sequence[i] << " -> ";
    std::cout << this->sequence.back() << std::endl;
}

Solution Solver::Solve(Data& d)
{
    return Solution();
}
