#include <iostream>
#include <memory>
#include <string>
#include <vector> 
#include <algorithm>
#include <set>
#include <utility>
#include <list>
#include <pthread.h>
#include <future>
#include <chrono>
#include "minisat/core/SolverTypes.h"
#include "minisat/core/Solver.h"

#define TIMEOUT 90
#define getarg struct arguments *args = (struct arguments *)argp; vector<vector<int>>& G = *(args->graph); vector<int>& res = *(args->result)

using namespace std;
pthread_t t1, tme;

struct arguments {
    vector<vector<int> >* graph;
    vector<int>* result;
};

void* t_t(void *argp){
    this_thread::sleep_for(chrono::seconds(TIMEOUT));
    pthread_cancel(t1);
}

void* VCSAT(void *argp){
    getarg;
    int n = G.size();
    vector<pair<int, int> > G_set;
    set<pair<int, int> > check;
    for(int i = 0; i < n; ++i){
        for(size_t j = 0; j < G[i].size(); ++j){
            if(check.find(make_pair(i, G[i][j])) == check.end()){
                G_set.push_back(make_pair(i, G[i][j]));
                check.insert(make_pair(i, G[i][j]));
                check.insert(make_pair(G[i][j], i));
            }
        }
    }

    pthread_create(&tme, NULL, &t_t, NULL);

    for(int k = 1; k <= n; ++k){
        pthread_testcancel();
        unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
        Minisat::Var X[n];
        Minisat::Var S[n][k];

        for (int i = 0; i < n; ++i) {
            X[i] = solver->newVar();
            for (int j = 0; j < k; ++j) {
                S[i][j] = solver->newVar();
            }
        }

        solver->addClause(~Minisat::mkLit(X[0]), Minisat::mkLit(S[0][0]));
        for(int j = 1; j < k; ++j){
            solver->addClause(~Minisat::mkLit(S[0][j]));
        }

        for (int i = 1; i < n - 1; ++i){
            solver->addClause(~Minisat::mkLit(X[i]), Minisat::mkLit(S[i][0]));
            solver->addClause(~Minisat::mkLit(S[i-1][0]), Minisat::mkLit(S[i][0]));
            solver->addClause(~Minisat::mkLit(X[i]), ~Minisat::mkLit(S[i-1][k-1]));
            for(int j = 1; j < k; ++j){
                solver->addClause(~Minisat::mkLit(X[i]), ~Minisat::mkLit(S[i-1][j-1]), Minisat::mkLit(S[i][j]));
                solver->addClause(~Minisat::mkLit(S[i-1][j]), Minisat::mkLit(S[i][j]));
            }
        }

        solver->addClause(~Minisat::mkLit(X[n-1]), ~Minisat::mkLit(S[n-2][k-1]));

        for (size_t i = 0; i < G_set.size(); ++i){
            solver->addClause(Minisat::mkLit(X[G_set[i].first]), Minisat::mkLit(X[G_set[i].second]));
        }

        if(solver->solve()){
            for(int i = 0; i < n; ++i){
                if(solver->modelValue(X[i]) == Minisat::l_True){
                    res.push_back(i);
                }   
            }
            break;
        }
        solver.reset(new Minisat::Solver());          
    }
    pthread_cancel(tme);
}

void* VC1(void *argp){
    getarg;
    while(!all_of(begin(G), end(G), [](const vector<int>& v ){return v.empty();})){
        res.push_back(max_element(begin(G), end(G), [](const vector<int>& l, const vector<int>& r){return l.size() < r.size();}) - G.begin());
        G[res.back()].clear();
        for(auto && vec: G){
            vec.erase(remove(vec.begin(), vec.end(), res.back()), vec.end());
        }
    }
}

void* VC2(void *argp){
    getarg;
    int p1, p2;
    while(!all_of(begin(G), end(G), [](const vector<int>& v){return v.empty();})){
        vector<int> tmp;
        vector<vector<int>>::iterator it = G.begin();
        while ((it = find_if(it, G.end(), [](const vector<int>& w) {return !w.empty();})) != G.end()){
            tmp.push_back(it++ - G.begin());
        }
        p1 = tmp[rand()%tmp.size()];
        p2 = G[p1][rand()%G[p1].size()];
        G[p1].clear();
        G[p2].clear();
        for(auto && vec: G){
            vec.erase(remove(vec.begin(), vec.end(), p1), vec.end());
            vec.erase(remove(vec.begin(), vec.end(), p2), vec.end());
        }
        res.push_back(p1);
        res.push_back(p2);
    }
}

void parseLine(string line, vector<int> &edges){ // the function cannot handle a line ending with a digit
    string num = "";
    for(string::size_type i = 0; i < line.size(); ++i) {
        if(isdigit(line[i])){
            num += line[i];
        }else{
            if(num.empty()){
                continue;
            }else{
                edges.push_back(stoi(num));
                num.clear();
            }
        }
    }
}

void initV(vector<vector<int> > &G, string line, int &v){
    v = stoi(line.erase(0, 2));
    for(vector<int> &w : G){
        w.clear();
    }
    G.clear();
    G.resize(v, vector<int>(0, 0));    
}

void initE(vector<vector<int> > &G, string line, int v){
    vector<int> edges;
    parseLine(line, edges);
    for(size_t i = 0; i < edges.size(); i += 2){
        if(find(G[edges[i]].begin(), G[edges[i]].end(), edges[i+1]) == G[edges[i]].end()) {
            G[edges[i]].push_back(edges[i+1]);
            G[edges[i+1]].push_back(edges[i]);
        } 
    }
}

void o(string name, vector<int>& res){
    sort(res.begin(), res.end());
    string delim = " ";
    cout << name;
    for(auto &&a: res){
        cout << delim << a;
        delim = ",";
    }
    cout << endl;
    res.clear();
}

int main(){
    int v = 0;
    string line;
    vector<vector<int> > G_SAT, G_VC1, G_VC2;
    vector<int> r1, r2, r3;
    while (getline(cin, line)){
        switch(line.at(0)){
            case 'V': 
            initV(G_SAT, line, v);
            initV(G_VC1, line, v);
            initV(G_VC2, line, v);
            break;

            case 'E':
            initE(G_SAT, line, v);
            initE(G_VC1, line, v);
            initE(G_VC2, line, v);
            pthread_t t2, t3;
            arguments* a1 = new arguments{&G_SAT, &r1};
            arguments* a2 = new arguments{&G_VC1, &r2};
            arguments* a3 = new arguments{&G_VC2, &r3};
            pthread_create(&t1, NULL, &VCSAT, a1);
            pthread_create(&t2, NULL, &VC1, a2);
            pthread_create(&t3, NULL, &VC2, a3);
            pthread_join(t1, NULL);
            pthread_join(t2, NULL);
            pthread_join(t3, NULL);

            if(r1.empty()){
                cout << "CNF-SAT-VC: timeout" << endl;
            }else{
                o("CNF-SAT-VC:", r1);
            }
            o("APPROX-VC-1:", r2);
            o("APPROX-VC-2:", r3);
            delete a1;
            delete a2;
            delete a3;
            break;
        }
    }
}