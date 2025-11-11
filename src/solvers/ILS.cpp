#include "solvers/ILS.h"

#define MAX_ITER 3
#define MAX_ITER_ILS 3

ILSSolver::ILSSolver()
{
    
}

ILSSolver::~ILSSolver()
{
    
}

Solution ILSSolver::Solve(Data& d)
{
    srand(time(NULL));
    this->current_data = &d;
    
    Solution bestOfAll;
    bestOfAll.cost = INFINITY;

    /*
    for(int i = 0; i < MAX_ITER; i++)
    {
        Solution s = this->Construcao();
        Solution best = s;

        int iterIls = 0;
        while(iterIls <= MAX_ITER_ILSSolver)
        {
            BuscaLocal(s);
            if(s.cost < best.cost)
            {
                best = s;
                iterIls = 0;
            }

            s = Pertubacao(best);
            iterIls++;
        }

        if(best.cost < bestOfAll.cost)
            bestOfAll = best;
    }
    */

    std::vector<int> seq = this->GetRandom3Sequence();
    std::cout << "max n: " << this->current_data->getDimension() << "|" << seq[0] << ", " << seq[1] << ", " << seq[2] << std::endl;
    
    return bestOfAll;
}


Solution ILSSolver::Construcao()
{

    return Solution();
}

Solution ILSSolver::Pertubacao(Solution& s)
{
    return Solution();
}

void ILSSolver::BuscaLocal(Solution& s)
{
   
}

std::vector<int> ILSSolver::GetRandom3Sequence()
{
    assert(this->current_data != nullptr);
    
    int dimension = this->current_data->getDimension();

    // p1: from 1 to n-2
    // p2: from p1 + 1 to n-1
    // p3: from p2 + 1 to n
    
    int p1 = rand() % (dimension - 2) + 1;    
    int p2 = rand() % ((dimension - 1) - p1) + p1 + 1;
    int p3 = rand() % (dimension - p2) + p2 + 1;
    
    std::vector<int> points = {p1, p2, p3};
    std::vector<int> result(3);
    
    // get shuffled result of p1/p2/p3
    int i = 0;
    while(points.size() > 0)
    {
        int chosen = rand() % points.size();
        result[i++] = points[chosen];
        points.erase(points.begin() + chosen);
    }

    return result;
}
