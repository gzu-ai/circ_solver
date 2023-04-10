//
// Created by 谢仲涛 on 2021/2/11.
//

#include <vector>
#include "LCMDCBDLSolverWrapper.h"

void CicrLF::LCMDCBDLSolverWrapper::addClause(std::vector<CicrLF::Lit> &clause) {
        Minisat::vec<Minisat::Lit> lits;
        lits.capacity(clause.size());
        std::for_each(clause.begin(), clause.end(), [&](CicrLF::Lit &d) {
            lits.push(Minisat::mkLit(d.value->value, d.neg));
        });
        _solver->addClause_(lits);
    }

    void CicrLF::LCMDCBDLSolverWrapper::setVars(int size) {
        while (_solver->nVars() < size)_solver->newVar();
    }

    void CicrLF::LCMDCBDLSolverWrapper::initSolver() {
        delete _solver;
        _solver = new Minisat::SimpSolver();
#ifdef USE_LCMDCBDL_SIMP
        _solver->eliminate(true);
#endif
    }




    const std::vector<char> &CicrLF::LCMDCBDLSolverWrapper::models() {
        Minisat::vec<Minisat::lbool> &m = _solver->model;
        _models.clear();
        for (int i = 0; i < m.size(); ++i) {
            _models.push_back(m[i] ==this->l_true);
        }
        return _models;
    }

CicrLF::LCMDCBDLSolverWrapper::LCMDCBDLSolverWrapper() {
    _solver = new Minisat::SimpSolver();
}

CicrLF::LCMDCBDLSolverWrapper::~LCMDCBDLSolverWrapper() {
    delete _solver;
}

void CicrLF::LCMDCBDLSolverWrapper::addClause(CicrLF::Lit &lit) {
    _solver->addClause(Minisat::mkLit(lit.value->value , lit.neg));
}

void CicrLF::LCMDCBDLSolverWrapper::addClause(unsigned int value, bool neg) {
    _solver->addClause(Minisat::mkLit(value , neg));

}

const std::vector<char> &CicrLF::LCMDCBDLSolverWrapper::models(int lim) {
    Minisat::vec<Minisat::lbool> &m = _solver->model;
    _models.clear();
    for (int i = 0; i < lim; ++i) {
        _models.push_back(m[i] ==this->l_true);
    }
    return _models;
}

void CicrLF::LCMDCBDLSolverWrapper::models(std::vector<char> &models) {
    Minisat::vec<Minisat::lbool> &m = _solver->model;
    models.clear();
    for (int i = 0; i < m.size(); ++i) {
        models.push_back(m[i] ==this->l_true);
    }
}

void CicrLF::LCMDCBDLSolverWrapper::models(int lim, std::vector<char> &model) {
    Minisat::vec<Minisat::lbool> &m = _solver->model;
    model.clear();
    for (int i = 0; i < lim; ++i) {
        model.push_back(m[i] ==this->l_true);
    }
}
