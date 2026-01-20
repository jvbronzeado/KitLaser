#include "solvers/BnC.h"
#include "solvers/ILS.h"
#include <algorithm>
#include <stdexcept>
#include <unordered_set>

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN
#include <map>
#include <math.h>
#include <algorithm>

#include "Branch-and-Cut/MyBranchCallback.h"
#include "Branch-and-Cut/MyCutCallback.h"
#include "Branch-and-Cut/MyLazyCallback.h"
#include "Branch-and-Cut/separation.h"

double getEdge(double** x, int i, int j) {
    if(i < j) {
        return x[i][j];
    }
    return x[j][i];
}

double getDegree(double** x, int i, int n) {
    double d = 0;
    for(int j = 0; j < n; j++) {
        if(i == j)
            continue;

        d += getEdge(x, i, j);
    }

    return d;
}

std::vector<int> MaxBackFromS0(double& out_cut, std::vector<int>& out_remainder, std::vector<int> s0, double** x, int n) {
    std::vector<double> maxback_val(n, 0.0);
    for(int v = 0; v < s0.size(); v++) {
        int i = s0[v];
        for(int j = 0; j < n; j++) {
            if(i != j) {
                maxback_val[j] += getEdge(x, i, j);
            }
        }
    }
    
    std::vector<bool> inset(n, false);
    for(int i = 0; i < s0.size(); i++) {
        inset[s0[i]] = true;
    }
    
    double cut_val = 0;

    for(int v = 0; v < s0.size(); v++) {
        int i = s0[v];
        for(int j = 0; j < n; j++) {
            if(i == j)
                continue;
            
            if(inset[j] == false) {
                cut_val += getEdge(x, i, j);
            }
        }
    }

    double mincut_val = cut_val;
    std::vector<int> Sk(s0);
    int bestcount = 0;

    int lastnode = -1;
    int penultimonode = -1;

    for(int k = 1; k <= n - s0.size(); k++) {
        // get maximum maxback_val
        int max_j = 0;
        double max_value = -std::numeric_limits<double>().infinity();
        for(int j = 0; j < n; j++) {
            if(inset[j] == false) {
                if(maxback_val[j] > max_value) {
                    max_value = maxback_val[j];
                    max_j = j;
                }
            }
        }

        // add max_j to the S array
        int i = max_j;

        penultimonode = lastnode;
        lastnode = i;
        
        if(inset[i] == false) {
            Sk.push_back(i);
            inset[i] = true;
        }

        // update cut_val
        cut_val += getDegree(x, i, n) - 2 * max_value;
        for(int j = 0; j < n; j++) {
            if(i == j)
                continue;
            
            if(inset[j] == false) {
                maxback_val[j] += getEdge(x, i, j);
            }
        }

        if(cut_val < mincut_val) {
            mincut_val = cut_val;
            bestcount = Sk.size();
        }
    }

    std::vector<int> s(bestcount);
    for(int i = 0; i < bestcount; i++) {
        s[i] = Sk[i];
    }

    if(penultimonode == -1) {
        penultimonode = s0[0];
    }

    out_remainder = {penultimonode, lastnode};

    out_cut = mincut_val;
    return s;
}

constexpr double epsilon = 1e-3;
vector<vector<int>> MaxBack(double** x, int n) {
    static constexpr int MAX_CUTS = 20;
    vector<vector<int>> cutPool;

    // q coisa feia meu deus
    priority_queue<pair<double, vector<int>>, vector<pair<double, vector<int>>>, greater<pair<double, vector<int>>>> best_values;
    
    for(int i = 0; i < n; i++) {
        double cut_value = 0;

        std::vector<int> remainder;
        vector<int> s = MaxBackFromS0(cut_value, remainder, {0}, x, n);

        if(cut_value < 2 - epsilon && s.size() >= 2 && s.size() <= n - 2) {
            best_values.push(make_pair(cut_value, s));
        }
    }

    for(int i = 0; i < MAX_CUTS && !best_values.empty(); i++) {
        pair<double, vector<int>> best = best_values.top();
        best_values.pop();

        cutPool.push_back(best.second);
    }
    
    return cutPool;
}

int shrink(double** x, std::vector<std::vector<int>>& nodemap, int n, int s, int t) {
    if(s == -1 || t == -1) {
        return 1;        
    }
    
    const int newn = n-1;

    nodemap[s].insert(nodemap[s].end(), nodemap[t].begin(), nodemap[t].end());
    nodemap.erase(nodemap.begin() + t);

    // cria uma copia do x
    double** tempx = new double*[n];
    for(int i = 0; i < n; i++) {
        tempx[i] = new double[n];
        for(int j = 0; j < n; j++) {
            tempx[i][j] = x[i][j];
        }
    }

    // calcula as novas distancias após remoção de t
    for(int i = 0; i < newn; i++) {
        for(int j = i + 1; j < newn; j++) {
            int oldi = i < t ? i : i + 1;
            int oldj = j < t ? j : j + 1;

            double value = getEdge(tempx, oldi, oldj);

            // como não é possível oldi e oldj ser t (pois se for igual a t então ele vira t + 1)
            // então só verificamos se um dos dois é s, se for, adiciona o valor de t também
            // x[i][s] + x[i][t]
            if(oldi == s) {
                value += getEdge(tempx, oldj, t);
            }
            else if(oldj == s) {
                value += getEdge(tempx, oldi, t);
            }

            x[i][j] = x[j][i] = value;
        }
    }

    // libera o x temporario
    for(int i = 0; i < n; i++) {
        delete[] tempx[i];
    }
    delete[] tempx;
    
    return newn;
}

vector<vector<int>> MinCut(double** x, int n) {
    double mincut_val = std::numeric_limits<double>().infinity();
    std::vector<int> bestS;

    std::vector<std::vector<int>> nodemap(n);
    for(int i = 0; i < n; i++) {
        nodemap[i] = {i};
    }

    int calcn = n;
    int shrinkcount = 0;
    while(calcn > 1) {
        double cut_value = 0;
        std::vector<int> remainder;
        vector<int> s = MaxBackFromS0(cut_value, remainder, {0}, x, calcn);

        if(cut_value < mincut_val) {
            mincut_val = cut_value;

            // converte o s para os indices verdadeiro deles com o nodemap
            bestS.clear();
            for(int j : nodemap[remainder[1]]) {
                bestS.push_back(j);
            }
        }

        calcn = shrink(x, nodemap, calcn, remainder[0], remainder[1]);
        shrinkcount++;
    }
    
    return {bestS};
}

Solution BNCSolver::Solve(Data& data)
{
    this->current_data = &data;
    int n = data.getDimension();

    // usa o ILS para resolver o TSP de forma heuristica
    Solution heuristico = ILSSolver().Solve(data);
    
    IloEnv env;
    IloModel model(env);

    env.setName("Branch and Cut");
    model.setName("Symmetrical Traveling Salesman Problem");

    int dimension = data.getDimension();

    /********** Creating variable x for each edge **********/
    IloArray <IloBoolVarArray> x(env, dimension);

    for (int i = 0; i < dimension; i++) {
		IloBoolVarArray array(env, dimension);
		x[i] = array;
	}
    /*******************************************************/

    /*********** Adding x variables to the model ***********/
    char var[100];
    for (int i = 0; i < dimension; i++){
        for (int j = i + 1; j < dimension; j++){
            sprintf(var, "X(%d,%d)", i, j);
			x[i][j].setName(var);
			model.add(x[i][j]);
        }
    }
    /******************************************************/
    
    /**************** Objective Function ******************/
    IloExpr obj(env);
    for (int i = 0; i < dimension; i++) {	
		for (int j = i + 1; j < dimension; j++) {
			obj += data.getDistance(i+1, j+1)*x[i][j];
		}
	}
    model.add(IloMinimize(env, obj));
    /******************************************************/
    
    /******************** Constraints *********************/
    IloRange r;
    char name[100];

    for (int i = 0; i < dimension; i++){
        IloExpr sumX(env);
        for (int j = 0; j < dimension; j++){
            if (j < i) {
				sumX += x[j][i];
			}
            if (i < j){
                sumX += x[i][j];
            }
        }
        r = (sumX == 2);
        sprintf(name, "c_%d", i);
		r.setName(name);
		model.add(r);
    }
    /******************************************************/

    /****************** Solve the model *******************/
    IloCplex STSP(model);
    STSP.setParam(IloCplex::TiLim, 2*60*60);
    STSP.setParam(IloCplex::Threads, 1);
    STSP.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 1e-08);
    STSP.setParam(IloCplex::CutUp, heuristico.cost);
    //STSP.exportModel("stsp.lp");

    double timeBefore, timeAfter;

    const IloArray<IloBoolVarArray>& x_ref = x;

    /********** Creating Branch Callback Object ***********/
    MyBranchCallback* branchCbk = new (env) MyBranchCallback(env);
    STSP.use(branchCbk);
    /******************************************************/

    /************ Creating Cut Callback Object ************/
    MyCutCallback* cutCbk = new (env) MyCutCallback(env, x_ref, dimension); 
	STSP.use(cutCbk);
    /******************************************************/

    /************ Creating Lazy Callback Object ***********/
    MyLazyCallback* lazyCbk = new (env) MyLazyCallback(env, x_ref, dimension);
    STSP.use(lazyCbk);
    /******************************************************/

    try{ 
	    timeBefore = STSP.getTime();
	    STSP.solve();
	    timeAfter = STSP.getTime();
    }
    catch(IloException& e){
        std::cout << e;
    }

    Solution solution;
    solution.cost = STSP.getBestObjValue();
    /******************************************************/

    /**************** Cleaning the memory *****************/
    delete branchCbk;
    delete cutCbk;
    delete lazyCbk;
    env.end();
    /******************************************************/
        
    return solution;
}
