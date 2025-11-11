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
    this->GenerateRandom3Sequence();
    std::vector<int>& seq = this->si;
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

void ILSSolver::GenerateRandom3Sequence()
{
    assert(this->current_data != nullptr);

    // automaticamente gera o s' e o CL, e salva na classe
    int dimension = this->current_data->getDimension();

    this->cl.resize(dimension);
    this->si.resize(3);

    for(int i = 0; i < dimension; i++)
    {
        this->cl[i] = i + 1;
    }

    for(int i = 0; i < 3; i++)
    {
        int chosen = rand() % this->cl.size();
        this->si[i] = this->cl[chosen];
        this->cl.erase(this->cl.begin() + chosen);
    }
}
