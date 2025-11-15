#include "solvers/MLP.h"

inline MLPSubsequence MLPSubsequence::Concatenate(Data* d, MLPSubsequence &sigma_2)
{
    MLPSubsequence output;
    double temp = d->getDistance(this->last, sigma_2.first);
    output.W = this->W + sigma_2.W;
    output.T = this->T + temp + sigma_2.T;
    output.C = this->C + sigma_2.W * (this->T + temp) + sigma_2.C;
    output.first = this->first;
    output.last = sigma_2.last;
    return output;
}

MLPSolver::MLPSolver()
{
    
}

MLPSolver::~MLPSolver()
{
    
}

Solution MLPSolver::Solve(Data& d)
{
    this->current_data = &d;
    
    Solution bestOfAll;
    bestOfAll.cost = INFINITY;

    int dimension = d.getDimension();
    
    const int max_iter = 10;
    const int max_iter_ils = std::min(100, dimension);

    for(int i = 0; i < max_iter; i++)
    {
        Solution s = this->Construcao();

        this->subseq_matrix = std::vector<std::vector<MLPSubsequence>>(d.getDimension() + 1, std::vector<MLPSubsequence>(d.getDimension() + 1));
        this->UpdateAllSubseq(s);

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
        {
            bestOfAll = best;
        }
    }

    return bestOfAll;
}


Solution MLPSolver::Construcao()
{
    int dimension = this->current_data->getDimension();
    
    // random num
    double alpha = (double)rand()/RAND_MAX;
    
    // initialize sequence and CL
    Solution s;
    s.sequence = {1};
    s.cost = 0;

    this->cl.resize(dimension - 1);
    for(int i = 2; i <= dimension; i++)
    {
        this->cl[i-2] = i;
    }

    int r = 1;
    Data* d = this->current_data;
    while(this->cl.empty() == false)
    {
        std::sort(this->cl.begin(), this->cl.end(), [&r, &d](int& a, int& b)
        {
            int dist_a = d->getDistance(r, a);
            int dist_b = d->getDistance(r, b);
            return dist_a < dist_b;
        });

        int max_i = std::ceil(alpha * this->cl.size());
        int chosen = rand() % max_i;

        s.sequence.push_back(this->cl[chosen]);

        r = this->cl[chosen];
        this->cl.erase(this->cl.begin() + chosen);
    }

    s.sequence.push_back(1);

    return s;
}

Solution MLPSolver::Pertubacao(Solution& s)
{
    Solution out;
    out.sequence = s.sequence;
    out.cost = s.cost;

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
    this->UpdateAllSubseq(out);

    return out;
}

void MLPSolver::BuscaLocal(Solution& s)
{
    std::vector<int> NL = {1, 2, 3, 4, 5};
    bool improved = false;

    while(NL.empty() == false)
    {
        int n = rand() % NL.size();
        improved = this->BestImprovementSwap(s);
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

void MLPSolver::UpdateAllSubseq(Solution& s)
{
    this->UpdateSubseqRange(s, 0, s.sequence.size() - 1);
}

void MLPSolver::UpdateSubseqRange(Solution& s, int begin, int end)
{
    int n = s.sequence.size();

    for(int i = begin; i <= end; i++)
    {
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s.sequence[i];
        subseq_matrix[i][i].last = s.sequence[i];
    }

    for(int i = 0; i <= end; i++)
    {
        for(int j = std::max(i + 1, begin); j < n; j++)
        {
            if(j < begin || i > end)
                continue;

            subseq_matrix[i][j] = subseq_matrix[i][j-1].Concatenate(this->current_data, subseq_matrix[j][j]);
        }
    }

    for(int i = n - 1; i >= begin; i--)
    {
        for(int j = std::min(end, i - 1); j >= 0; j--)
        {
            if(i < begin || j > end)
                continue;
            subseq_matrix[i][j] = subseq_matrix[i][j+1].Concatenate(this->current_data, subseq_matrix[j][j]);
        }
    }

    s.cost = subseq_matrix[0][n-1].C;
}

bool MLPSolver::BestImprovementSwap(Solution& s)
{
    assert(this->current_data != nullptr);

    double best_cost = this->subseq_matrix[0][s.sequence.size() - 1].C;
    size_t best_i = 0, best_j = 0;
    bool improved = false;
    for(size_t i = 1; i < s.sequence.size() - 2; i++)
    {
        for(size_t j = i + 1; j < s.sequence.size() - 1; j++)
        {
            double delta = 0.0;
            if(j == i + 1)
            {
                MLPSubsequence sigma_1 = this->subseq_matrix[0][i-1].Concatenate(this->current_data, this->subseq_matrix[j][j]);
                MLPSubsequence sigma_2 = sigma_1.Concatenate(this->current_data, this->subseq_matrix[i][i]);
                MLPSubsequence sigma_3 = sigma_2.Concatenate(this->current_data, this->subseq_matrix[j + 1][s.sequence.size()-1]);
                delta = sigma_3.C;
            }
            else
            {
                MLPSubsequence sigma_1 = this->subseq_matrix[0][i-1].Concatenate(this->current_data, this->subseq_matrix[j][j]);
                MLPSubsequence sigma_2 = sigma_1.Concatenate(this->current_data, this->subseq_matrix[i + 1][j - 1]);
                MLPSubsequence sigma_3 = sigma_2.Concatenate(this->current_data, this->subseq_matrix[i][i]);
                MLPSubsequence sigma_4 = sigma_3.Concatenate(this->current_data, this->subseq_matrix[j + 1][s.sequence.size()-1]);
                delta = sigma_4.C;
            }

            if(delta < best_cost)
            {
                best_cost = delta;
                best_i = i;
                best_j = j;
                improved = true;
            }
        }
    }

    if(improved)
    {
        std::swap(s.sequence[best_i], s.sequence[best_j]);

        this->UpdateSubseqRange(s, best_i, best_j);
        return true;
    }

    return false;
}

bool MLPSolver::BestImprovementOPTOPT(Solution& s)
{
    double best_cost = this->subseq_matrix[0][s.sequence.size() - 1].C;
    size_t best_i, best_j;
    bool improved = false;
    for(size_t i = 0; i < s.sequence.size() - 2; i++)
    {
        for(size_t j = i + 2; j < s.sequence.size() - 1; j++)
        {
            MLPSubsequence sigma_1 = this->subseq_matrix[0][i].Concatenate(this->current_data, this->subseq_matrix[j][i + 1]);
            MLPSubsequence sigma_2 = sigma_1.Concatenate(this->current_data, this->subseq_matrix[j+1][s.sequence.size()-1]);
            if(sigma_2.C < best_cost)
            {
                best_cost = sigma_2.C;
                best_i = i;
                best_j = j;
                improved = true;
            }
        }
    }

    if(improved)
    {
        // apply from best_i + 1 to best_j in reverse_order
        std::reverse(s.sequence.begin() + best_i + 1, s.sequence.begin() + best_j + 1);

        // apply new cost
        s.cost = best_cost;
        this->UpdateSubseqRange(s, best_i, best_j+1);
        return true;
    }

    return false;
}

bool MLPSolver::BestImprovementOrOpt(Solution& s, uint8_t len)
{
    assert(this->current_data != nullptr);

    double best_cost = this->subseq_matrix[0][s.sequence.size() - 1].C;
    size_t best_i, best_j;
    bool improved = false;
    for(size_t i = 1; i < s.sequence.size() - 1 - len; i++)
    {
        for(size_t j = 0; j < s.sequence.size() - 1; j++)
        {
            if(j >= i - 1 && j < i + len)
            {
                continue;
            }

            double delta = 0;
            if(j < i)
            {
                MLPSubsequence sigma_1 = this->subseq_matrix[0][j].Concatenate(this->current_data, this->subseq_matrix[i][i+len-1]);
                MLPSubsequence sigma_2 = sigma_1.Concatenate(this->current_data, this->subseq_matrix[j+1][i-1]);
                MLPSubsequence sigma_3 = sigma_2.Concatenate(this->current_data, this->subseq_matrix[i+len][s.sequence.size()-1]);
                delta = sigma_3.C;
            }
            else
            {
                MLPSubsequence sigma_1 = this->subseq_matrix[0][i-1].Concatenate(this->current_data, this->subseq_matrix[i+len][j]);
                MLPSubsequence sigma_2 = sigma_1.Concatenate(this->current_data, this->subseq_matrix[i][i+len-1]);
                MLPSubsequence sigma_3 = sigma_2.Concatenate(this->current_data, this->subseq_matrix[j+1][s.sequence.size()-1]);
                delta = sigma_3.C;
            }
                        
            if(delta < best_cost)
            {
                best_cost = delta;
                best_i = i;
                best_j = j;
                improved = true;
            }
        }
    }

    if(improved)
    {
        if(best_j < best_i)
        {
            std::rotate(s.sequence.begin() + best_j + 1, s.sequence.begin() + best_i, s.sequence.begin() + best_i + len);
            this->UpdateSubseqRange(s, best_j, best_i + len - 1);
        }
        else
        {
            std::rotate(s.sequence.begin() + best_i, s.sequence.begin() + best_i + len, s.sequence.begin() + best_j + 1);
            this->UpdateSubseqRange(s, best_i, best_j);
        }

        return true;
    }

    return false;
}
