//
// Created by 谢仲涛 on 2021/2/20.
//

#include <cstring>
#include "Application.h"
//#include "CLI11.hpp"
//
//
namespace CicrLF{
    void split(const std::string& str, const std::string& delim,std::vector<std::string> & out) {
        if(str.empty()) return ;
        //先将要切割的字符串从string类型转换为char*类型
        char * strs = new char[str.length() + 1] ; //不要忘了
        char * pp =strs;
        strcpy(strs, str.c_str());

        char * d = new char[delim.length() + 1];
        strcpy(d, delim.c_str());

        char *p = strtok(strs, d);
        while(p) {
            std::string s = p; //分割得到的字符串转换为string类型
            out.push_back(s); //存入结果数组
            p = strtok(nullptr, d);
        }
//        delete pp;
    }
    void parseAtom(std::string& input,std::vector<std::string>& output){
        if (input.empty())return;
        std::ifstream t(input.c_str());
        if (t.fail()){
            std::cerr<<"WARNING: "<< input<< " maybe not a file" <<std::endl;
            split(input,",",output);
            return;
        }
        std::stringstream buffer;
        buffer << t.rdbuf();
        std::string contents(buffer.str());
        split(buffer.str(),",",output);
    }

    Application * Application::_instance= nullptr;
    void Application::parseOption(int argc,char **argv) {
        CLI::App app{"compute cicr", APP_NAME};
        std::string minimal;
        std::string varied;
        std::string fixed;
        app.add_option("--minimal,-m", minimal, "minimal atoms spite by \",\"", false)
                ->required(false);
        app.add_option("--varied,-v", varied, "varied atoms spite by \",\"", false);
        app.add_option("--fixed,-f", fixed, "fixed atoms spite by \",\"", false);
        app.add_option("input-file", options.input_file, "input file")
                ->required(true)
                ->check(CLI::ExistingFile);
        app.add_option("--output,-o", options.output_file, "output file", true)->required(false);
        app.add_option("--cpu-limit", options.cpu_limit, "cpu limit", true)->required(false);
        app.add_option("--mem-limit", options.mem_limit, "mem limit", true)->required(false);
        app.add_option("--number,-n", options.number, "models number", true)->required(false);
        app.add_flag_callback("--version", [&]() {
            std::cout << Solver::description <<" v" APP_VERSION<< std::endl;
            exit(0);
        });
        try {
            (app).parse((argc), (argv));
            parseAtom(minimal,options.minimal);
            parseAtom(varied,options.varied);
            parseAtom(fixed,options.fixed);
            options.has_fixed=!options.fixed.empty();
            options.has_minimal=!options.minimal.empty();
            options.has_varied=!options.varied.empty();
        } catch (const CLI::ParseError &e) {
            exit(app.exit(e));
        }
    }

    int Application::run(int argc, char **argv) {
        status.runTime=realTime();
        status.cpuTime=cpuTime();
        printInfo();
        parseOption(argc, argv);
        setup();
        status.step=PARSE;
        if (!parseFile(options.input_file)) {
            std::cerr << "input file format error" << std::endl;
            exit(0);
        }
        double start=cpuTime();
        status.step=SOLVE;
        try {
            if (options.has_varied)clauseSet.setVaried(options.varied);
            if (options.has_minimal)clauseSet.setMinimal(options.minimal);
            if (options.has_fixed)clauseSet.setFixed(options.fixed);
            clauseSet.prepare();

            solver->preSolve();
            status = status + solver->status();
            solver->resetStatus();
            if (!solver->solve()) {
                status.result=UNSAT;
                status = status + solver->status();
                status.cpuTime= cpuTime(); //-;
                status.runTime=realTime()-status.runTime;
                _resultWriter->printStatus(status);
                return 0;
            }
            status.result=SAT;
            do {
                status = status + solver->status();
                solver->resetStatus();
                modelsList.push_back(solver->models());
                ++status.models;
                if (status.models == options.number) {
                    status.more=true;
                    break;
                }

            } while (solver->solveMore());
            status = status + solver->status();
            status.cpuTime= cpuTime()-start; //-;
            status.runTime=realTime()-status.runTime;
            std::for_each(modelsList.begin(), modelsList.end(),[&](std::vector<char>&m){_resultWriter->printModel(m,this->clauseSet.vars());});
            _resultWriter->printStatus(status);
            return 0;
        } catch (std::exception &) {
            status.result=INTERRUPT;
            status = status + solver->status();
            status.cpuTime= cpuTime(); //-;
            status.runTime=realTime()-status.runTime;
            _resultWriter->printStatus(status);
        }
        return 100;
    }

    inline bool Application::parseFile(
            const std::string &input_path) noexcept {
        try {
            status.parseTime=realTime();
            double start =cpuTime();
            DIMACSParser parser(input_path.data());
            parser.parse(clauseSet);
            status.parseTime=realTime()-status.parseTime;
            status.cpuTime+=(cpuTime()-start);
        } catch (std::exception& e) {
            std::cout<< e.what() <<std::endl;
            return false;
        }
        return true;
    }

    inline void Application::setup() {
        _resultWriter=new ResultWriter(options.output_file);
        CicrLF::limitTime(options.cpu_limit);
        CicrLF::limitMemory(options.mem_limit);
        signal(SIGXCPU,&Application::sigHandler);
    }

    void Application::sigHandler(int s) {
        double cpuT= cpuTime();
        double time= realTime();
        ApplicationStatus  status=_instance->status;
        SolveStatus sta= _instance->solver->status();
        if (status.step==PARSE){
            status.parseTime=time- status.parseTime;
            status.cpuTime=cpuT- status.cpuTime;
        } else{
            if (sta.running){
                sta.cpuTime=cpuT-sta.cpuTime;
                sta.solveTime=time-sta.solveTime;
            }
            status=status+sta;
        }
        _instance->_resultWriter->printStatus(status);
        exit(s);
    }

    void Application::printInfo() {
        std::cout << Solver::description <<" v" APP_VERSION<< std::endl;
        puts("Search engine: " SAT_SOLVER"(" SAT_SOLVER_VERSION ")");
    }
}