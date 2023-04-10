//
// Created by 谢仲涛 on 2021/2/11.
//

#ifndef CICR_LF_MINISATSOLVERWRAPPER_H
#define CICR_LF_MINISATSOLVERWRAPPER_H


#include "SolverWrapper.h"
#include "minisat/core/Solver.h"
#include "../core/CluaseSet.h"
namespace CicrLF {
    class MiniSATSolverWrapper {
    public:
        void addClause(std::vector<Lit> &clause);
        void addClause(Lit &clause);
        void addClause(unsigned int value,bool neg );

        MiniSATSolverWrapper();

        void setVars(int size);

        inline void clear() {initSolver(); }

        ~MiniSATSolverWrapper();

        const std::vector<char> &models();
        void models(std::vector<char>& model);
        void models(int lim,std::vector<char>& model);
        const std::vector<char> &models(int lim);
        inline bool isMinimal(){return false;}
        inline bool solve() {
            bool result =_solver->solve();
            return result;
        }

    private:
        Minisat::Solver *_solver;
        std::vector<char> _models;

        void initSolver();
    };
}
#endif //CICR_LF_MINISATSOLVERWRAPPER_H
