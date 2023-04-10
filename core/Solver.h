//
// Created by xiezt on 2021/11/17.
//

#ifndef CICR_LF_CIRCREDUCTSOLVER_H
#define CICR_LF_CICRSOLVER_H

#include <utils/SystemUtils.h>
#include <algorithm>
#include "CluaseSet.h"
#include "Status.h"
#include "ClauseExpander.h"
#include <config.h>
namespace CicrLF {
    class Solver {
    private:
        ClauseSet &_orginClauseSet;
        ClauseSet _clauseSet;
        Wrapper *_satSolver;
        Wrapper * _variedSolver;

        std::vector<char> _model;
        SolveStatus _status;
        bool _doSolve();
        bool solveVaried();
    public:
        static char* description;
        ~Solver();
        Solver(ClauseSet&clauseSet) : _orginClauseSet(clauseSet) {
            _satSolver=new Wrapper();
            _variedSolver=new Wrapper();
        }


        void preSolve(){
            _clauseSet.copyFrom(_orginClauseSet);
        }
        inline SolveStatus& status(){return _status;}
//        const inline ClauseSet &getClauseSet() { return _clauseSet; }
        bool solve();
        inline void resetStatus(){
            _status.solveTime=0;
            _status.cpuTime=0;
            _status.loops=0;
            _status.callSat=0;
        }

        const inline std::vector<char> &models() {
            return _model;
        }

        bool solveMore();
    };

}


#endif //CICR_LF_CIRCREDUCTSOLVER_H
