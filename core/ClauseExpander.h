//
// Created by 谢仲涛 on 2021/2/13.
//

#ifndef CICR_LF_CLAUSEEXPANDER_H
#define CICR_LF_CLAUSEEXPANDER_H

#include <vector>
#include "CluaseSet.h"
namespace CicrLF {
    /**
     * 子句扩展
    */
    class ClauseExpander {
    public:
        explicit ClauseExpander(int initVar = 0) { setInitVar(initVar); }

        void setInitVar(int initVar) {
            nextVar = initVar;
            innerVar=0;
        }

        void imply(Atom * left, std::vector<Clause *> &right,std::vector<Clause *> & result) {
            if (right.empty()) return;
            std::vector<Lit> * lits=new std::vector<Lit>();
            Clause* clause=new Clause{lits};
            lits->push_back({left, true});
            std::for_each(right.begin(), right.end(), [&](Clause *c) {
                lits->push_back(!transform(*c, result));
            });
            result.push_back(clause);
        }
        void imply(std::vector<Atom*>& left, std::vector<Clause *> &right,std::vector<Clause *> & result) {
            std::vector<Lit> * lits=new std::vector<Lit>();
            Clause* clause=new Clause{lits};
            std::for_each(left.begin(), left.end(), [&](Atom *a) { lits->push_back({a, true}); });
            std::for_each(right.begin(), right.end(), [&](Clause *it) {
                lits->push_back(!transform(*it, result));
            });
            result.push_back(clause);
        }

        int maxVar() const { return nextVar; }

    private:
        unsigned int nextVar;
        int innerVar;
        int innerSize=0;
        std::vector<Atom *> _atoms;
        Atom * makeNewAtom(){
            Atom * atom;
            if (innerVar<innerSize){
                atom=_atoms[innerVar++];
            } else{
                atom=new Atom{nextVar,UNKOWN, false};
                _atoms.push_back(atom);
                ++innerSize;
                ++innerVar;
            }
            atom->value=nextVar++;
            return atom;
        }


        inline Lit transform(Clause &clause, std::vector<Clause*> &result) {
            if (clause.clause->size() == 1) {
                return clause.clause->back();
            }
            Lit r{makeNewAtom(), false};
            std::vector<Lit>*lits =new std::vector<Lit>();
            Clause*tmp=new Clause{lits};
            lits->push_back(!r);
            std::copy(clause.clause->begin(), clause.clause->end(), std::back_inserter(*lits));
            result.push_back(tmp);
            std::for_each(clause.clause->begin(), clause.clause->end(), [&](Lit &lit) {
                std::vector<Lit>*lits =new std::vector<Lit>();
                Clause*tmp=new Clause{lits};
                lits->push_back(!lit);
                lits->push_back(r);
                result.push_back(tmp);
            });
            return r;

        }

    };
}

#endif //CICR_LF_CLAUSEEXPANDER_H
