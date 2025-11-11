#ifndef SOLVER_H_
#define SOLVER_H_

#include <iostream>
#include <vector>

#include "../../include/Data.h"

struct Solution {
    std::vector<int> sequence;
    double cost;

    void Print();
};

class Solver {
public:
    Solver() = default;
    virtual ~Solver() = default;

    virtual Solution Solve(Data& d);
private:
    
};

#endif
