#ifndef SOLVER_H_
#define SOLVER_H_

#include <vector>

struct Solution {
    std::vector<int> sequence;
    double value;

    void Print();
};

class Solver {
public:
    Solver() = default;
    ~Solver() = default;

    virtual Solution Solve();
private:
    
};

#endif
