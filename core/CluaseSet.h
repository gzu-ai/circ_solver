//
// Created by 谢仲涛 on 2021/2/11.
//

#ifndef CICR_LF_CLUASESET_H
#define CICR_LF_CLUASESET_H
#include "TypeDefine.h"
#include <list>
#include <vector>
#include <deque>
#include <iostream>
#include <algorithm>
namespace CicrLF {
    class ClauseSet {

    private:

        ClausesType *_clauses = nullptr;
        unsigned int _atomSize = 0;
        int _clauseSize = 0;
        InputType _inputType;
        std::vector<Atom *> _minimal;
        std::vector<Atom *> _varied;
        std::vector<Atom *> _fixed;
        std::vector<Atom *> _atoms;
        int _mask = 0;

    public:
        inline int size() const { return _clauseSize; }
        friend void redact(ClauseSet& s,std::vector<Atom*>& X,std::vector<Atom*>& Y);
        ClauseSet(const ClauseSet &set);
        void copyFrom(const ClauseSet &set);
        void copySimple(const ClauseSet &set);
        void print();

        inline void setInputType(InputType inputType) { _inputType = inputType; }

        inline void reserve(unsigned int size) { _clauses->reserve(size); }

        Lit makeLit(unsigned int value, bool neg);

        inline int vars() const { return _atomSize; }

        inline const std::vector<Atom *> &getMinimal() { return _minimal; }

        inline const std::vector<Atom *> &getVaried() { return _varied; }

        inline const std::vector<Atom *> &getFixed() { return _fixed; }

        inline void setMinimal(std::vector<std::string> &labels) {
            _mask = _mask | 0b1;
            labelToVar(labels, _minimal, MINIMAL);
        }
        inline void setVaried(std::vector<std::string> &labels) {
            _mask = _mask | 0b10;
            labelToVar(labels, _varied, VARIABLE);
        }

        inline void setFixed(std::vector<std::string> &labels) {
            _mask = _mask | 0b100;
            labelToVar(labels, _fixed, FIXED);
        }



        void completeAtom(std::vector<Atom *> &, AtomType);

        void prepare() {
            if ((_mask & 0b001) == 0) {
                completeAtom(_minimal, MINIMAL);
            }
            if (_mask == 1) {
                completeAtom(_fixed, FIXED);
            }
//            if (_mask==0b010){
//                completeAtom(_minimal,MINIMAL);
//            }
//            if (_mask==0b100){
//                completeAtom(_minimal,MINIMAL);
//            }
            if (_mask == 0b011) {
                completeAtom(_fixed, FIXED);
            }
            if (_mask == 0b101) {
                completeAtom(_varied, VARIABLE);
            }
        }

        bool labelToVar(std::vector<std::string> &labels, std::vector<Atom *> &vars, AtomType type) {
            vars.clear();
            errno = 0;
            if (_inputType == CNF) {
                for (std::string &label: labels) {

                    int atom = (int) strtol(label.c_str(), nullptr, 10) - 1;
                    if (errno) {
                        std::cerr << "can not parse: " << label << std::endl;
                        exit(-1);
                    }
                    if (atom > _atomSize) {
                        std::cerr << "can find atom: " << label << std::endl;
                        exit(-1);
                    }
                    Atom *a = _atoms[atom];
                    a->type = type;
                    vars.push_back(a);
                }
            }
            if (_inputType == LP) {
                std::cerr << "lp format didnt have been implement yet!" << std::endl;
                exit(-1);
            }
            return true;
        }

        ClausesType &getClause() { return *_clauses; }

        void addClause(Clause *claus);

        Clause*  addClause(std::vector<Lit> *lits);

        inline typename ClausesType::iterator
        begin() { return _clauses->begin(); }

        inline typename ClausesType::iterator
        end() { return _clauses->end(); }
        std::vector<Clause*>& getClauses();
        Atom * getAtom(unsigned int);

        ~ClauseSet();
        ClauseSet() {
            _clauses = new ClausesType();
        }
    };
}
#endif //CICR_LF_CLUASESET_H
