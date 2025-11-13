#include "solvers/ILS.h"
#include <chrono>

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
    this->current_data = &d;
    
    Solution bestOfAll;
    bestOfAll.cost = INFINITY;

    int dimension = d.getDimension();
    
    const int max_iter = 50;
    const int max_iter_ils = dimension >= 150 ? (dimension >> 1) : dimension;

    for(int i = 0; i < max_iter; i++)
    {
        Solution s = this->Construcao();
        Solution best = s;

        int iterIls = 0;
        while(iterIls <= max_iter_ils)
        {
            this->BuscaLocal(s);
            if(s.cost < best.cost)
            {
                best = s;
                iterIls = 0;
            }

            s = this->Pertubacao(best);
            iterIls++;
        }

        if(best.cost < bestOfAll.cost)
            bestOfAll = best;
    }
    
    return bestOfAll;
}


Solution ILSSolver::Construcao()
{
    this->GenerateRandom3Sequence();

    Solution s;
    s.sequence = this->si;
    s.cost = 0;

    s.sequence.push_back(s.sequence.front());

    double** c = this->current_data->getMatrixCost();
    for (size_t i = 0; i < s.sequence.size() - 1; i++)
        s.cost += c[s.sequence[i]-1][s.sequence[i + 1]-1];

    while(this->cl.empty() == false)
    {
        this->GenerateInsertionCosts(s);
        std::sort(this->insertion_costs.begin(), this->insertion_costs.end(), compare_ils_insertion_infos);

        double alpha = (double)rand()/RAND_MAX;
        int max_i = std::ceil(alpha * this->insertion_costs.size());
        int chosen = rand() % max_i;

        ILSInsertionInfo& info = this->insertion_costs[chosen];
        s.sequence.insert(s.sequence.begin() + info.removed_edge + 1, cl[info.node_index]);
        
        s.cost += this->insertion_costs[chosen].cost;
        this->cl.erase(this->cl.begin() + this->insertion_costs[chosen].node_index);
    }

    return s;
}

Solution ILSSolver::Pertubacao(Solution& s)
{
    Solution out;
    out.sequence = s.sequence;

    // calculate block 1 and block 2 length
    int dim = this->current_data->getDimension();
    
    int s1len = rand() % std::max(1, dim/10 - 1) + 2;
    int s2len = rand() % std::max(1, dim/10 - 1) + 2;

    size_t seq_size = s.sequence.size() - 2; // ignore start/end

    // calculate where block 1 and block 2 will start
    int s1start = rand() % (seq_size - s2len - s1len + 1) + 1;
    int s2start = rand() % (seq_size - (s1start - 1) - s1len - s2len + 1) + s1start + s1len;

    // swap blocks
    int block_finish = s2start + s2len;
    std::reverse(out.sequence.begin() + s1start, out.sequence.begin() + block_finish);
    std::reverse(out.sequence.begin() + s1start, out.sequence.begin() + s1start + s2len);
    std::reverse(out.sequence.begin() + s1start + s2len, out.sequence.begin() + block_finish - s1len);
    std::reverse(out.sequence.begin() + block_finish - s1len, out.sequence.begin() + block_finish);

    // calculate cost
    out.cost = 0;
    for(size_t i = 0; i < out.sequence.size() - 1; i++)
    {
        out.cost += this->current_data->getDistance(out.sequence[i], out.sequence[i + 1]);
    }

    return out;
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
            improved = this->BestImprovementSwap(s);
            break;
        case 2:
            improved = this->BestImprovementOPTOPT(s);
            break;
        case 3:
            improved = this->BestImprovementOrOpt(s, 1);
            break;
        case 4:
            improved = this->BestImprovementOrOpt(s, 2);
            break;
        case 5:
            improved = this->BestImprovementOrOpt(s, 3);
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
            this->insertion_costs[l].cost = c[i-1][k-1] + c[k-1][j-1] - c[i-1][j-1];
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
    for(size_t i = 1; i < s.sequence.size() - 2; i++)
    {
        int vi = s.sequence[i];
        int vi_next = s.sequence[i + 1];
        int vi_prev = s.sequence[i - 1];
        for(size_t j = i + 1; j < s.sequence.size() - 1; j++)
        {
            int vj = s.sequence[j];
            int vj_next = s.sequence[j + 1];
            int vj_prev = s.sequence[j - 1];

            // the delta needs to be handled differently when i and j are adjacents,
            // or else delta will most of time time be negative
            double delta = 0.0;
            if(j == i + 1)
            {
                delta = c[vi_prev-1][vj-1] + c[vi-1][vj_next-1] + c[vj-1][vi-1]
                        - c[vi_prev-1][vi-1] - c[vi-1][vj-1] - c[vj-1][vj_next-1];
            }
            else
            {
                // if i and j were adjacents here
                // vi_next would be j, and vj_prev would be i
                // meaning c[vi][vi_next] would be removed twice because of c[vj_prev][vj]
                // and c[vj][vi_next] and c[vj_prev][vi] would both be equal to 0
                // resulting on a delta lesser than 0
                delta = -c[vi_prev-1][vi-1] - c[vi-1][vi_next-1] + c[vi_prev-1][vj-1]
                        + c[vj-1][vi_next-1] - c[vj_prev-1][vj-1] - c[vj-1][vj_next-1]
                        + c[vj_prev-1][vi-1] + c[vi-1][vj_next-1];
            }

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

bool ILSSolver::BestImprovementOPTOPT(Solution& s)
{
    assert(this->current_data != nullptr);
    double** c = this->current_data->getMatrixCost();

    double best_delta = 0;
    size_t best_i, best_j;
    for(size_t i = 0; i < s.sequence.size() - 2; i++)
    {
        int vi = s.sequence[i];
        int vi_next = s.sequence[i + 1];
        // n pode arestas adjacentes, por isso o i + 2
        for(size_t j = i + 2; j < s.sequence.size() - 1; j++)
        {
            int vj = s.sequence[j];
            int vj_next = s.sequence[j + 1];

            double delta = -c[vi-1][vi_next-1] - c[vj-1][vj_next-1] + c[vi-1][vj-1] + c[vi_next-1][vj_next-1];
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
        // apply from best_i + 1 to best_j in reverse_order
        std::reverse(s.sequence.begin() + best_i + 1, s.sequence.begin() + best_j + 1);

        // apply new cost
        s.cost += best_delta;
        return true;
    }

    return false;
}

bool ILSSolver::BestImprovementOrOpt(Solution& s, uint8_t len)
{
    assert(this->current_data != nullptr);
    double** c = this->current_data->getMatrixCost();

    double best_delta = 0;
    size_t best_i, best_j;
    for(size_t i = 1; i < s.sequence.size() - 1 - len; i++)
    {
        int vi = s.sequence[i];
        int vi_next = s.sequence[i + len];
        int vi_prev = s.sequence[i - 1];
        int vib_last = s.sequence[i + len - 1];
        for(size_t j = 0; j < s.sequence.size() - 1; j++)
        {
            if(j >= i - 1 && j < i + len)
            {
                continue;
            }
            
            int vj = s.sequence[j];
            int vj_next = s.sequence[j + 1];

            double delta = c[vj-1][vi-1] + c[vib_last-1][vj_next-1] + c[vi_prev-1][vi_next-1]
                           - c[vi_prev-1][vi-1] - c[vib_last-1][vi_next-1] - c[vj-1][vj_next-1];
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
        if(best_j < best_i)
        {
            std::rotate(s.sequence.begin() + best_j + 1, s.sequence.begin() + best_i, s.sequence.begin() + best_i + len);
        }
        else
        {
            std::rotate(s.sequence.begin() + best_i, s.sequence.begin() + best_i + len, s.sequence.begin() + best_j + 1);
        }

        s.cost += best_delta;

        return true;
    }

    return false;
}
