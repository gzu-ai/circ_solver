//
// Created by 谢仲涛 on 2021/2/11.
//

#include <vector>
#include "CluaseSet.h"

CicrLF::Lit CicrLF::ClauseSet::makeLit(unsigned int value, bool neg=false) {
    reserve(value);
    while (_atomSize <= value)_atoms.push_back(makeAtom(_atomSize++));
    Atom* atom =this->_atoms[value];
    return Lit{atom,neg};
}

void CicrLF::ClauseSet::addClause(CicrLF::Clause *claus) {
    _clauseSize++;
    _clauses->push_back(claus);

}

CicrLF::Clause*  CicrLF::ClauseSet::addClause(std::vector<CicrLF::Lit> *lits) {
    Clause* cls=new Clause{lits};
    addClause(cls);
    return cls;
}

void CicrLF::ClauseSet::completeAtom(std::vector<CicrLF::Atom *> & out,AtomType type) {
    out.clear();
    std::for_each(_atoms.begin(), _atoms.end(),[&](Atom * atom){
       if(atom->type == UNKOWN){
           atom->type =type;
           out.push_back(atom);
       }
    });
}


CicrLF::ClauseSet::ClauseSet(const CicrLF::ClauseSet&obj) {
    _clauses= new ClausesType(obj._clauseSize);
    this->copyFrom(obj);
}

CicrLF::ClauseSet::~ClauseSet() {
    for (int i = 0; i < _clauseSize; ++i) {
        Clause * cls =_clauses->operator[](i);

        delete cls->clause;
        delete cls;
        _clauses->operator[](i)=nullptr;
    }
    for (int i = 0; i < _atomSize; ++i) {
        delete _atoms[i];
    }
    delete _clauses;

}

std::vector<CicrLF::Clause *> &CicrLF::ClauseSet::getClauses() {
    return *_clauses;
}

CicrLF::Atom *CicrLF::ClauseSet::getAtom(unsigned  int value) {
    return _atoms[value];
}

void CicrLF::ClauseSet::copyFrom(const CicrLF::ClauseSet &obj) {
    _clauseSize=obj._clauseSize;
    _clauses->resize(_clauseSize);
    _atoms.resize(obj._atomSize);
    std::for_each(obj._atoms.begin(),obj._atoms.end(),[&](Atom *atom){
        Atom * a=makeAtom(atom->value,atom->type);
        _atoms[atom->value]=a;
        switch (a->type){
            case MINIMAL: _minimal.push_back(a);break;
            case FIXED: _fixed.push_back(a);break;
            case VARIABLE: _varied.push_back(a);break;
            default:break;
        }
    });
    for (int i=0;i<_clauseSize;i++){
        std::vector<Lit> *clause= obj._clauses->operator[](i)->clause;
        std::vector <Lit> *lits=new std::vector <Lit>(clause->size());
        Clause * new_clause=new Clause{lits};
        for (int j=0;j<clause->size();j++){
            Lit& lit= clause->operator[](j);
            Atom*atom= _atoms[lit.value->value];
            Lit& new_lit=lits->operator[](j);
            new_lit.value=atom;
            new_lit.neg=lit.neg;
            if (lit.neg){
                atom->negative.push_back(new_clause);
            } else{
                atom->positive.push_back(new_clause);
            }
        }
        _clauses->operator[](i)=new_clause;
    }
    this->_mask=obj._mask;
    _atomSize=obj._atomSize;
    _inputType=obj._inputType;
}

void CicrLF::ClauseSet::copySimple(const CicrLF::ClauseSet &obj) {
    _atoms.resize(obj._atomSize);
    std::for_each(obj._atoms.begin(),obj._atoms.end(),[&](Atom *atom){
        Atom * a=makeAtom(atom->value,atom->type);
        _atoms[atom->value]=a;
        switch (a->type){
            case MINIMAL: _minimal.push_back(a);break;
            case FIXED: _fixed.push_back(a);break;
            case VARIABLE: _varied.push_back(a);break;
            default:break;
        }
    });
    this->_mask=obj._mask;
    _atomSize=obj._atomSize;
    _inputType=obj._inputType;
}

void CicrLF::ClauseSet::print() {
    for (int i=0;i<_clauseSize;i++){
        std::vector<Lit> *clause= _clauses->operator[](i)->clause;
        for (int j=0;j<clause->size();j++){
            Lit& lit= clause->operator[](j);
            if (lit.neg){
                std::cout<< "-";
            }else{
                std::cout<< " ";
            }
            std::cout<< lit.value->value+1 << " ";
        }
        std::cout<<std::endl;
    }
}


