//
// Created by xiezt on 2022/11/21.
//

#include "LoopFormula.h"

CicrLF::ClausesType &CicrLF::DnfLoopFormula::compute(CicrLF::ClauseSet &eliminated,
                                                     const std::vector<Atom *> &loop,
                                                     CicrLF::ClausesType &out) {
    std::vector<char> map = std::vector<char>();
    map.resize(eliminated.vars(), false);
    for (Atom *atom: loop) {
        map[atom->value] = true;
    }
    for (Clause *clause: eliminated) {
        std::vector<Lit> *lits = new std::vector<Lit>;
        for (Atom *atom:loop) {
            lits->push_back({atom,true});
        }
//        lits->push_back({atom,true});
        for (Lit lit: *clause->clause) {
            if (map[lit.value->value]) {
                if (lit.neg) {
                    delete lits;
                    lits = nullptr;
                    break;

                } else {
                    continue;
                }
            } else {
                lits->push_back({lit.value,!lit.neg});
            }
        }
        if (lits!=nullptr){
            CicrLF::Clause* sig=new Clause{lits};
            out.push_back(sig);
        }
    }

}
