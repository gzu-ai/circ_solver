//
// Created by 谢仲涛 on 2021/2/11.
//

#ifndef CICR_LF_LCMDCBDLSOLVERWRAPPER_H
#define CICR_LF_LCMDCBDLSOLVERWRAPPER_H


#include "SolverWrapper.h"
#include "simp/SimpSolver.h"
#include "core/SolverTypes.h"
#include "core/TypeDefine.h"
#include "utils/SystemUtils.h"


namespace CicrLF {
    class LCMDCBDLSolverWrapper {
    public:
        void addClause(std::vector<CicrLF::Lit> &clause);
        void addClause(CicrLF::Lit &lit);
        void addClause(unsigned int value,bool neg );
        LCMDCBDLSolverWrapper();

        void setVars(int size);

        inline void clear() { initSolver(); }

        ~LCMDCBDLSolverWrapper();

        const std::vector<char> &models();
        void models(std::vector<char>& model);
        void models(int lim,std::vector<char>& model);
        const std::vector<char> &models(int lim);


        inline bool solve() {
            auto time= cpuTime();
            bool result = _solver->solve();
          //  printf("solve %0.5f \n",cpuTime()-time);
            return result;
        }

    private:
        Minisat::SimpSolver *_solver;
        std::vector<char> _models;

        void initSolver();

        const Minisat::lbool l_true = Minisat::lbool((uint8_t) 0);
    };
}
#endif //CICR_LF_LCMDCBDLSOLVERWRAPPER_H
