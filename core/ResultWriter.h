//
// Created by 谢仲涛 on 2021/2/21.
//

#ifndef CICR_LF_RESULTWRITER_H
#define CICR_LF_RESULTWRITER_H


#include <cstdio>
#include <string>
#include <utils/SystemUtils.h>
#include "core/Status.h"
namespace CicrLF {
    class ResultWriter {

    public:
        explicit ResultWriter(const std::string& file){
            if (file!="-"){
                fp=fopen(file.c_str(),"a+");
            }
            if (fp==nullptr){
                fp=stdout;
            }
        }
        ~ResultWriter(){if (fp!=stdout)fclose(fp);}
        template<class T>
        void printModel(const T &model, int size) {
            ++models;
            fprintf(fp,"Model %d: \n", models);
            for (int i = 0; i < size; ++i) {
                if (model[i])fprintf(fp,"%d ", i + 1);
            }
            fprintf(fp,"\n");
            fflush(fp);
        }

        template<class T>
        void printStatus(const T &status) {
            if (status.result==INTERRUPT){
                fprintf(fp,"INTERRUPT\n\n");
            } else if (status.result==SAT){
                fprintf(fp,"SATISFIABLE\n\n");
            } else{
                fprintf(fp,"UNSATISFIABLE\n\n");
            }
            double time = status.parseTime + status.solveTime;
            fprintf(fp,"Time          : %0.5fs  \n", time);
            fprintf(fp,"eliminateTime : %0.5fs  \n", status.eliminateTime);
            fprintf(fp,"Run Time      : %0.5fs  \n", status.runTime);
            fprintf(fp,"CPU Time      : %0.5fs  \n", status.cpuTime);
            fprintf(fp,"Models        : %d", status.models);
            if (status.more)fprintf(fp,"+\n"); else fprintf(fp,"\n");
            fprintf(fp,"SAT Call      : %d\n", status.callSat);
            fprintf(fp,"Memory        : %0.3fMb \n", memUsed());
        }

    private:
        int models = 0;
        FILE *fp= nullptr;

    };

}
#endif //CICR_LF_RESULTWRITER_H
