//
// Created by 谢仲涛 on 2021/2/13.
//

#ifndef CICR_LF_SETUTILS
#define CICR_LF_SETUTILS


#include "core/CluaseSet.h"

namespace CicrLF {

    /**
     *
     * @param s
     * @param X  atoms be T
     * @param Y
     */
    void redact(ClauseSet& s,std::vector<Atom*>& X,std::vector<Atom*>& Y) {
        std::for_each(X.begin(), X.end(),[&](Atom* atom){
            atom->flag= true;
            for (auto &item : atom->positive){
                delete item->clause;
                item->clause= nullptr;
            }
        });
        std::for_each(Y.begin(), Y.end(),[&](Atom* atom){
            atom->flag= true;
            for (auto &item : atom->negative){
                delete item->clause;
                item->clause= nullptr;
            }
        });
        std::vector<Clause*>& clauses=s.getClauses();
        int e=s.size();
        for (int i = 0; i <e; ++i) {
            Clause * start = clauses[i];
            if (start->clause== nullptr){
                while (clauses[--e]->clause== nullptr&i <e);
                if (e==i)break;
                clauses[i]=clauses[e];
                clauses[e]=start;
                start = clauses[i];
            }
            if (e==i)break;
            int m=start->clause->size();
            for (int j = 0; j < m; ++j) {
                Lit  lit = start->clause->operator[](j);
                if (!lit.value->flag){
                    continue;
                }
                while (start->clause->operator[](--m).value->flag&&j < m);
                if (m==j)break;
                start->clause->operator[](j)=start->clause->operator[](m);
                start->clause->operator[](m)=lit;
            }
            start->clause->resize(m);
        }
        clauses.resize(e);
        s._clauseSize=e;
    }
    // 返回 false 说明是无效环
    bool eliminatedClauses(std::vector<Clause *>& input,std::vector<Clause *> &reslut){

        for (const auto &item : input){
            if (item->clause== nullptr)continue;
            if (item->emt)continue;
            std::vector<Lit> * lits=new std::vector<Lit>();
            Clause *clause=new Clause{lits,false};
            bool flag=false;
            for (const auto &lit : *item->clause){
                if (lit.value->flag){
                    if (lit.neg){
                        item->emt=true;
                        flag=true;
                        break;
                    }
                    continue;
                }
                lits->push_back(lit);
            }
            if (flag){
                delete lits;
                delete clause;
                continue;
            }
            if (lits->empty()){
                delete lits;
                delete clause;
                return false;
            }
            reslut.push_back(clause);
        }
        return true;
    }
// 返回 false 说明是无效环
    bool eliminatedClauses(ClauseSet& s,std::vector<Atom*>& X,std::vector<Clause *> &reslut ){
        bool success=true;
        std::for_each(X.begin(), X.end(),[&](Atom* atom) {
            atom->flag = true;
        });

        for (Atom* atom :X) {
            if (!eliminatedClauses(atom->positive, reslut)){
                success=false;
                break;
            }
        }
        std::for_each(X.begin(), X.end(),[&](Atom* atom) {
            atom->flag = false;
            for (const auto &item : atom->positive){
                item->emt=false;
            }
        });
        return  success;
    }


    // 返回 false 说明是无效环
    bool eliminatedNDFClauses(std::vector<Clause *>& input,std::vector<Clause *> &reslut){

        for (const auto &item : input){
            if (item->clause== nullptr)continue;
            if (item->emt)continue;
            std::vector<Lit> * lits=new std::vector<Lit>();
            Clause *clause=new Clause{lits,false};
            bool flag=false;
            for (const auto &lit : *item->clause){
                if (lit.value->flag){
                    if (lit.neg){
                        item->emt=true;
                        flag=true;
                        break;
                    }
                    continue;
                }
                lits->push_back(lit);
            }
            if (flag){
                delete lits;
                delete clause;
                continue;
            }
            if (lits->empty()){
                delete lits;
                delete clause;
                return false;
            }
            reslut.push_back(clause);
        }
        return true;
    }
// 返回 false 说明是无效环
    bool eliminatedDNFClauses(ClauseSet& s,std::vector<Atom*>& X,std::vector<Clause *> &reslut ){
        bool success=true;
        std::for_each(X.begin(), X.end(),[&](Atom* atom) {
            atom->flag = true;
        });

        for (Atom* atom :X) {
            if (!eliminatedClauses(atom->positive, reslut)){
                success=false;
                break;
            }
        }
        std::for_each(X.begin(), X.end(),[&](Atom* atom) {
            atom->flag = false;
            for (const auto &item : atom->positive){
                item->emt=false;
            }
        });
        return  success;
    }

}
#endif //CICR_LF_SETUTILS_H
