//
// Created by 谢仲涛 on 2021/2/21.
//

#ifndef CICR_LF_MINIMALSATSOLVER_H
#define CICR_LF_MINIMALSATSOLVER_H
#include "minimal/MMSolver.h"
#include <vector>
#include "core/CluaseSet.h"
namespace CicrLF {
    class MinimalSATSolver {
    public:
        void addClause(std::vector<Lit> &clause);

        MinimalSATSolver();

        void setVars(int size);

        inline void clear() {  }

        ~MinimalSATSolver();

        const std::vector<char> &models();
        inline bool isMinimal(){return true;}
        inline bool solve() {
            delete _solver;
            _solver=new Minimal::MMSolver();
            while (_solver->nVars() < _nVars)_solver->newVar();
            for(auto & clause:_clauses){
                Minisat::vec<Minisat::Lit> lits;
                lits.capacity(clause.size());
                std::for_each(clause.begin(), clause.end(), [&](Lit &d) { lits.push(Minisat::mkLit(d.value - 1, d.neg)); });
                _solver->addClause(lits);
            }
            bool result =_solver->solve();
            if (result){
                Minisat::vec<Minisat::lbool> &m = _solver->getModel();
                _models.clear();
                for (int i = 0; i < m.size(); ++i) {
                    _models.push_back(m[i] == Minisat::l_True);
                }
            }
            return result;
        }

    private:
        Minimal::MMSolver *_solver;
        std::vector<char> _models;
        std::vector<std::vector<Lit>> _clauses;
        int _nVars;

    };
}


#endif //CICR_LF_MINIMALSATSOLVER_H
