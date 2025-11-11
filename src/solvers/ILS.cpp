#include "../../include/solvers/ILS.h"

ILS::ILS()
{
    
}

ILS::~ILS()
{
    
}

Solution ILS::Solve(Data& d)
{
    Solution bestOfAll;
    bestOfAll.cost = INFINITY;

    for(int i = 0; i < MAX_ITER; i++)
    {
        Solution s = this->Construcao();
        Solution best = s;

        int iterIls = 0;
        while(iterIls <= MAX_ITER_ILS)
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

    return bestOfAll;
}

Solution ILS::Construcao()
{
    return Solution();
}

Solution ILS::Pertubacao(Solution& s)
{
    return Solution();
}

void ILS::BuscaLocal(Solution& s)
{
   
}
