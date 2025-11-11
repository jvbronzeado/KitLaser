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
    s.cost = 0;
    
    while(this->cl.empty() == false)
    {
        this->GenerateInsertionCosts(s);
        std::sort(this->insertion_costs.begin(), this->insertion_costs.end(), compare_ils_insertion_infos);

        double alpha = (double)rand()/RAND_MAX;
        int max_i = std::ceil(alpha * this->insertion_costs.size());
        int chosen = rand() % max_i;

        s.sequence.push_back(this->cl[this->insertion_costs[chosen].node_index]);
        s.cost += this->insertion_costs[chosen].cost;
        this->cl.erase(this->cl.begin() + this->insertion_costs[chosen].node_index);
    }
    
    return s;
}

Solution ILSSolver::Pertubacao(Solution& s)
{
    return Solution();
}

void ILSSolver::BuscaLocal(Solution& s)
{
    std::vector<int> NL = {1, 2, 3, 4, 5};
    bool improved = false;

    while(NL.empty() == false)
    {
        int n = rand() % NL.size();
        switch(NL[n])
        {
        case 1:
            improved = BestImprovementSwap(s);
            break;
        case 2: // TODO
        case 3: // TODO
        case 4: // TODO
        case 5: // TODO
            improved = BestImprovementSwap(s);
            break;
        }

        if(improved)
        {
            NL = {1, 2, 3, 4, 5};
        }
        else
        {
            NL.erase(NL.begin() + n);
        }
    }
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

void ILSSolver::GenerateInsertionCosts(Solution& s)
{
    assert(this->current_data != nullptr);

    double** c = this->current_data->getMatrixCost();

    this->insertion_costs.resize((s.sequence.size() - 1) * (this->cl.size()));
    size_t l = 0;
    for(size_t a = 0; a < s.sequence.size() - 1; a++)
    {
        int i = s.sequence[a];
        int j = s.sequence[a + 1];
        for(size_t b = 0; b < this->cl.size(); b++)
        {
            int k = this->cl[b];
            this->insertion_costs[l].cost = c[i][k] + c[j][k] - c[i][j];
            this->insertion_costs[l].node_index = b;
            this->insertion_costs[l].removed_edge = a;
            l++;
        }
    }
}

bool ILSSolver::BestImprovementSwap(Solution& s)
{
    assert(this->current_data != nullptr);
    double** c = this->current_data->getMatrixCost();

    double best_delta = 0;
    size_t best_i, best_j;
    for(size_t i = 1; i < s.sequence.size() - 1; i++)
    {
        int vi = s.sequence[i];
        int vi_next = s.sequence[i + 1];
        int vi_prev = s.sequence[i - 1];
        for(size_t j = i + 1; j < s.sequence.size() - 1; j++)
        {
            int vj = s.sequence[j];
            int vj_next = s.sequence[j + 1];
            int vj_prev = s.sequence[j - 1];
            double delta = -c[vi_prev][vi] - c[vi][vi_next] + c[vi_prev][vj]
                    + c[vj][vi_next] - c[vj_prev][vj] - c[vj][vj_next]
                    + c[vj_prev][vi] + c[vi][vj_next];

            if(delta < best_delta)
            {
                best_delta = delta;
                best_i = i;
                best_j = j;
            }
        }
    }

    if(best_delta < 0)
    {
        std::swap(s.sequence[best_i], s.sequence[best_j]);
        s.cost += best_delta;
        return true;
    }

    return false;
}
