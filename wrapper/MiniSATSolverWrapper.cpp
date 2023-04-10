//
// Created by 谢仲涛 on 2021/2/11.
//

#include "MiniSATSolverWrapper.h"
namespace CicrLF {
    void MiniSATSolverWrapper::addClause(std::vector<Lit> &clause) {
        Minisat::vec<Minisat::Lit> lits;
        lits.capacity(clause.size());
        std::for_each(clause.begin(), clause.end(), [&](Lit &d) {
            lits.push(Minisat::mkLit(d.value->value , d.neg));
        });
        _solver->addClause_(lits);
    }

    MiniSATSolverWrapper::MiniSATSolverWrapper() {
        _solver = new Minisat::Solver();
    }

    void MiniSATSolverWrapper::setVars(int size) {
        while (_solver->nVars() < size)_solver->newVar();
    }

    void MiniSATSolverWrapper::initSolver() {
        delete _solver;
        _solver = new Minisat::Solver();
#ifdef USE_MINISAT_SIMP
        _solver->eliminate(true);
#endif
    }

    MiniSATSolverWrapper::~MiniSATSolverWrapper() {
        delete _solver;
    }


    const std::vector<char> &MiniSATSolverWrapper::models() {
        Minisat::vec<Minisat::lbool> &m = _solver->model;
        _models.clear();
        for (int i = 0; i < m.size(); ++i) {
            _models.push_back(m[i] == Minisat::l_True);
        }
        return _models;
    }

    void MiniSATSolverWrapper::addClause(Lit &lit) {
        _solver->addClause(Minisat::mkLit(lit.value->value , lit.neg));
    }

    void MiniSATSolverWrapper::addClause(unsigned int value, bool neg) {
        _solver->addClause(Minisat::mkLit(value , neg));
    }

    void MiniSATSolverWrapper::models(std::vector<char> &model) {
        Minisat::vec<Minisat::lbool> &m = _solver->model;
        model.clear();
        for (int i = 0; i < m.size(); ++i) {
            model.push_back(m[i] ==Minisat::l_True);
        }
    }

    void MiniSATSolverWrapper::models(int lim, std::vector<char> &model) {
        Minisat::vec<Minisat::lbool> &m = _solver->model;
        model.clear();
        for (int i = 0; i < lim; ++i) {
            model.push_back(m[i] ==Minisat::l_True);
        }
    }

    const std::vector<char> &MiniSATSolverWrapper::models(int lim) {
        Minisat::vec<Minisat::lbool> &m = _solver->model;
        _models.clear();
        for (int i = 0; i < lim; ++i) {
            _models.push_back(m[i] == Minisat::l_True);
        }
        return _models;
    }

}