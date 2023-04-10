//
// Created by 谢仲涛 on 2021/2/21.
//

#include "MinimalSATSolver.h"

void CicrLF::MinimalSATSolver::addClause(std::vector<Lit> &clause) {
    _clauses.push_back(clause);
}

CicrLF::MinimalSATSolver::MinimalSATSolver() {
}

void CicrLF::MinimalSATSolver::setVars(int size) {
    _nVars=size;
}

CicrLF::MinimalSATSolver::~MinimalSATSolver() {
    delete _solver;
}

const std::vector<char> &CicrLF::MinimalSATSolver::models() {
    return _models;
}
