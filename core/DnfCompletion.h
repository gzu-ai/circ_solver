//
// Created by 谢仲涛 on 2021/2/13.
//

#ifndef CICR_LF_COMPLETION_H
#define CICR_LF_COMPLETION_H


#include "CluaseSet.h"
#include "ClauseExpander.h"
#include "utils/SetUtils.h"

namespace CicrLF {
    class Completion {

    public:
        Completion(ClauseExpander &clauseExpander) : _clauseExpander(clauseExpander) {}

        ClausesType &compute(CicrLF::ClauseSet &eliminated,const std::vector<Atom *>& atoms);

        inline ClausesType &getCompClauses() { return _clauses; }
        inline void clear(){
            for (Clause* it:_clauses) {
                delete it;
            }
            _clauses.clear();
        }

    private:
        ClauseExpander &_clauseExpander;
        ClausesType _clauses;
    };

//
//    CicrLF::ClausesType &Completion::compute(CicrLF::ClauseSet &eliminated,const std::vector<Atom *>& atoms) {
//        _clauses.clear();
//        auto time= cpuTime();
//        std::vector<Atom *> singleP(1);
//        for (Atom * atom: atoms) {
//            singleP[0]=atom;
//            ClausesType tmp;
//            auto success= eliminatedClauses(eliminated,singleP,tmp);
//            if (!success)continue;
//            if (!tmp.empty()) {
//                _clauseExpander.imply(atom, tmp, _clauses);
//            }else{
//                std::vector<Lit> * lits=new std::vector<Lit>(1);
//                CicrLF::Clause* sig=new Clause{lits};
//                lits->operator[](0)=Lit{atom,true};
//                _clauses.push_back(sig);
//            }
//        }
//        printf("comp time %.5f  size %d\n",cpuTime()-time,_clauses.size());
//
//        return _clauses;
//    }


    CicrLF::ClausesType &Completion::compute(CicrLF::ClauseSet &eliminated,const std::vector<Atom *>& atoms) {
        for (Atom *atom : atoms) {
            for (auto  item:eliminated) {
                std::vector<Lit> * lits=new std::vector<Lit>;
                lits->push_back({atom,true});
                for (auto it:*item->clause){
                    if (it.value==atom){
                        if (it.neg){
                            delete lits;
                            lits=nullptr;
                            break;
                        }else{
                            continue;
                        }
                    }else{
                        lits->push_back({it.value,!it.neg});
                    }
                }
                if (lits!=nullptr){
                    CicrLF::Clause* sig=new Clause{lits};
                    _clauses.push_back(sig);
                }

            }
        }
        return _clauses;
    }
}
#endif //CICR_LF_COMPLETION_H
