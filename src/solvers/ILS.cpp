#include "solvers/ILS.h"

#define MAX_ITER 5
#define MAX_ITER_ILS 5

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

    for(int i = 0; i < MAX_ITER; i++)
    {
        Solution s = this->Construcao();
        Solution best = s;

        int iterIls = 0;
        while(iterIls <= MAX_ITER_ILS)
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

    bestOfAll.Print();
    std::cout << "Cost: " << bestOfAll.cost << std::endl;
    return bestOfAll;
}


Solution ILSSolver::Construcao()
{
    this->GenerateRandom3Sequence();

    Solution s;
    s.sequence = this->si;
    s.cost = 0;

    double** c = this->current_data->getMatrixCost();
    for (size_t i = 0; i < s.sequence.size() - 1; i++)
        s.cost += c[s.sequence[i]][s.sequence[i + 1]];

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

    s.cost += c[s.sequence.back()][s.sequence.front()];    
    s.sequence.push_back(s.sequence.front());

    return s;
}

Solution ILSSolver::Pertubacao(Solution& s)
{
    return s;
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

    this->insertion_costs.clear();
    this->insertion_costs.resize((s.sequence.size() - 1) * (this->cl.size()));
    size_t l = 0;
    for(size_t a = 0; a < s.sequence.size() - 1; a++)
    {
        int i = s.sequence[a];
        int j = s.sequence[a + 1];
        for(size_t b = 0; b < this->cl.size(); b++)
        {
            int k = this->cl[b];
            this->insertion_costs[l].cost = c[i][k] + c[k][j] - c[i][j];
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
                delta = c[vi_prev][vj] + c[vi][vj_next] + c[vj][vi]
                        - c[vi_prev][vi] - c[vi][vj] - c[vj][vj_next];
            }
            else
            {
                // if i and j were adjacents here
                // vi_next would be j, and vj_prev would be i
                // meaning c[vi][vi_next] would be removed twice because of c[vj_prev][vj]
                // and c[vj][vi_next] and c[vj_prev][vi] would both be equal to 0
                // resulting on a delta lesser than 0
                delta = -c[vi_prev][vi] - c[vi][vi_next] + c[vi_prev][vj]
                        + c[vj][vi_next] - c[vj_prev][vj] - c[vj][vj_next]
                        + c[vj_prev][vi] + c[vi][vj_next];
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

            double delta = -c[vi][vi_next] - c[vj][vj_next] + c[vi][vj] + c[vi_next][vj_next];
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
        int left_ptr = best_i + 1;
        int right_ptr = best_j;

        while(left_ptr < right_ptr)
        {
            std::swap(s.sequence[left_ptr], s.sequence[right_ptr]);
            left_ptr++;
            right_ptr--;
        }

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
        for(size_t j = i + len; j < s.sequence.size() - 1; j++)
        {
            int vj = s.sequence[j];
            int vj_next = s.sequence[j + 1];

            double delta = c[vj][vi] + c[vib_last][vj_next] + c[vi_prev][vi_next]
                           - c[vi_prev][vi] - c[vib_last][vi_next] - c[vj][vj_next];
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
        std::rotate(s.sequence.begin() + best_i, s.sequence.begin() + best_i + len, s.sequence.begin() + best_j + 1);
        s.cost += best_delta;
        return true;
    }

    return false;
}
