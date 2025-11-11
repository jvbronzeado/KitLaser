#include "solvers/ILS.h"

#define MAX_ITER 3
#define MAX_ITER_ILS 3

bool compare_ils_insertion_infos(const ILSInsertionInfo& a, const ILSInsertionInfo& b)
{
    return a.cost < b.cost;
}

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

    Solution newsol = this->Construcao();
    newsol.Print();
    return bestOfAll;
}


Solution ILSSolver::Construcao()
{
    this->GenerateRandom3Sequence();

    Solution s;
    s.sequence = this->si;
    
    for(size_t i = 0; i < this->cl.size(); i++)
    {
        this->GenerateInsertionCosts(s, i);
        std::sort(this->insertion_costs.begin(), this->insertion_costs.end(), compare_ils_insertion_infos);

        double alpha = (double)rand()/RAND_MAX;
        int max_i = std::ceil(alpha * this->insertion_costs.size());
        int chosen = rand() % max_i;
        s.sequence.push_back(this->insertion_costs[chosen].inserted_node);
    }
    
    return s;
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
    int dimension = this->current_data->getDimension()-1;

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

void ILSSolver::GenerateInsertionCosts(Solution& s, size_t starting_i)
{
    assert(this->current_data != nullptr);

    double** c = this->current_data->getMatrixCost();

    this->insertion_costs.resize((s.sequence.size() - 1) * (this->cl.size() - starting_i));
    size_t l = 0;
    for(size_t a = 0; a < s.sequence.size() - 1; a++)
    {
        int i = s.sequence[a];
        int j = s.sequence[a + 1];
        for(size_t b = starting_i; b < this->cl.size(); b++)
        {
            int k = this->cl[b];
            this->insertion_costs[l].cost = c[i][k] + c[j][k] - c[i][j];
            this->insertion_costs[l].inserted_node = k;
            this->insertion_costs[l].removed_edge = a;
            l++;
        }
    }
}
