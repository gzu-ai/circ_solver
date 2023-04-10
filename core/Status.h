//
// Created by 谢仲涛 on 2021/2/21.
//

#ifndef CICR_LF_STATUS_H
#define CICR_LF_STATUS_H

namespace CicrLF {
    enum SolveStep{
        PARSE,SOLVE
    };

    enum ResultType{
        SAT,UNSAT,INTERRUPT
    };

    struct SolveStatus {
        double solveTime=0;
        double cpuTime=0;
        int callSat=0;
        double eliminateTime=0;
        int loops=0;
        bool running= false;
        inline SolveStatus operator+(SolveStatus &status) const{
            SolveStatus s;

            s.solveTime = solveTime + status.solveTime;
            s.cpuTime = cpuTime + status.cpuTime;
            s.callSat = callSat + status.callSat;
            s.loops = loops + status.loops;
            s.eliminateTime=eliminateTime+status.eliminateTime;
            status.solveTime = 0;
            status.cpuTime = 0;
            status.callSat =0;
            status.loops = 0;
            status.eliminateTime = 0;
            status.running = false;
            return s;
        }
    };
    struct ApplicationStatus {
        double parseTime=0;
        double solveTime=0;
        double cpuTime=0;
        double runTime=0;
        double eliminateTime=0;
        int callSat=0;
        int loops=0;
        int models=0;
        bool more= false;
        ResultType result=INTERRUPT;
        SolveStep step=PARSE;
        inline ApplicationStatus operator+(SolveStatus &status) const {
            ApplicationStatus s;
            s.solveTime = solveTime + status.solveTime;
            s.cpuTime = cpuTime + status.cpuTime;
            s.callSat = callSat + status.callSat;
            s.loops = loops + status.loops;
            s.eliminateTime = eliminateTime + status.eliminateTime;
            s.models = models;
            s.more=more;
            s.step=step;
            s.result=result;
            s.runTime=runTime;
            status.solveTime = 0;
            status.cpuTime = 0;
            status.callSat =0;
            status.loops = 0;
            status.running = false;
            status.eliminateTime=0;
            return s;
        }
    };
}

#endif //CICR_LF_STATUS_H
