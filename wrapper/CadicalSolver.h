//
// Created by 谢仲涛 on 2021/3/6.
//

#ifndef CICR_LF_CADICALSOLVER_H
#define CICR_LF_CADICALSOLVER_H

#include "cadical.hpp"
#include <vector>
#include "core/CluaseSet.h"
namespace CicrLF {
    class CadicalSolver {
    public:
        inline void addClause(std::vector<Lit> &clause){
            std::for_each(clause.begin(),clause.end(),[&](Lit&lit){
               addClause(lit);
            });
            _solver->add(0);
        }
        void addClause(CicrLF::Lit &lit){
            addClause(lit.value->value,lit.neg);
        }
        void addClause(unsigned int value,bool neg ){
            int v=value+1;
            int vv=neg?-v:v;
            _solver->add(vv);
        }

        CadicalSolver(){ clear();}

        inline void setVars(int size){_nVars=size;}

        inline void clear() {
            delete _solver;
            _solver=new CaDiCaL::Solver;
        }

        ~CadicalSolver(){delete _solver;}

        const inline std::vector<char> &models(){return models(_solver->vars());}

        inline bool isMinimal() { return false; }

         bool solve() {
            bool result = _solver->solve()==10;
//            if (result) {
//                _models.clear();
//                int i = 1;
//                int c= _solver->vars();
//                for (; i < c+1; ++i) {
//                    _models.push_back(_solver->val(i)>0);
//                }
//                for (--i; i < _nVars; ++i) {
//                    _models.push_back(false);
//                }
//
//            }
            return result;
        }
        const std::vector<char>&models(int vars) {
            _models.clear();
            int i = 1;
            int c= vars;
            for (; i < c+1; ++i) {
                _models.push_back(_solver->val(i)>0);
            }
            for (--i; i < _nVars; ++i) {
                _models.push_back(false);
            }
            return _models;
        }
        void models(std::vector<char> &_models,int vars){
            _models.clear();
            int i = 1;
            int c= vars;
            for (; i < c+1; ++i) {
                _models.push_back(_solver->val(i)>0);
            }
            for (--i; i < _nVars; ++i) {
                _models.push_back(false);
            }
        }
        void models(std::vector<char> &m){
            models(m,_solver->vars());
        }
    private:
        CaDiCaL::Solver *_solver= nullptr;
        std::vector<char> _models;
        std::vector<std::vector<Lit>> _clauses;
        int _nVars;
    };

}
#endif //CICR_LF_CADICALSOLVER_H
