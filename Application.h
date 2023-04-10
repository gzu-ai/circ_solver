//
// Created by 谢仲涛 on 2021/2/20.
//

#ifndef CICR_LF_APPLICATION_H
#define CICR_LF_APPLICATION_H
#include <string>
#include <vector>
#include <config.h>
#include <core/Status.h>
#include <core/ResultWriter.h>
#include <csignal>
#include "utils/Parser.h"
#include "core/Solver.h"
#include "core/CluaseSet.h"
#include "CLI11.hpp"
#include "utils/SystemUtils.h"
#include "utils/DIMACSParser.h"

namespace CicrLF {
    struct ApplicationOption {
        std::vector<std::string> minimal;
        std::vector<std::string> varied;
        std::vector<std::string> fixed;
        bool has_minimal;
        bool has_varied;
        bool has_fixed;
        std::string input_file;
        std::string output_file = "-";
        rlim_t cpu_limit = 0;
        rlim_t mem_limit = 0;
        unsigned int number = 1;
    };

    class Application {

    public:
        int run(int argc, char *argv[]);

        Application() noexcept {
            solver = new Solver(clauseSet);
            _instance = this;
        }

        ~Application() noexcept {
            delete solver;
            delete _resultWriter;
        }

        static Application *_instance;

    private:
        CicrLF::ClauseSet clauseSet;
        Solver *solver;
        ApplicationOption options;

        static void printInfo();

        void setup();

        std::vector<std::vector<char>> modelsList;

        void parseOption(int argc, char *argv[]);

        static void sigHandler(int);

        static Application &getInstance() { return *_instance; }

        ApplicationStatus status;
        ResultWriter *_resultWriter;

        bool parseFile(const std::string &input_path) noexcept;

    };
}



#endif //CICR_LF_APPLICATION_H
