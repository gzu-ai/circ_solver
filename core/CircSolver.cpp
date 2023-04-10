//
// Created by xiezt on 2021/11/17.
//

#include "Solver.h"
#include "utils/SetUtils.h"
char*  CicrLF::Solver::description="only use recursion";

// circ recursion
bool CicrLF::Solver::_doSolve() {
    ClauseSet  T=_clauseSet;
    this->_model.clear();
    _satSolver->setVars(_clauseSet.vars());
    std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
    ++_status.callSat;
    if (!_satSolver->solve())return false;
    std::vector<char> model = _satSolver->models();
    const std::vector<Atom*> &fixed = T.getFixed();
    const std::vector<Atom*> &minimal = T.getMinimal();
    std::vector<Lit> lits;
    std::vector<Lit> Y;
    std::vector<Lit> Tt;
    for (Atom * atom: fixed) {
        unsigned  int value =atom->value;
        Y.push_back({atom,model[value]==false});
    }
    for (Atom * atom: minimal) {
        unsigned  int value =atom->value;
        if (model[value]){
            lits.push_back({atom, true});
        }else{
            Tt.push_back({atom,true});
        }
    }
    _satSolver->clear();
    _satSolver->setVars(T.vars());
    std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
    _satSolver->addClause(lits);
    std::for_each(Y.begin(), Y.end(), [&](Lit &c) { _satSolver->addClause(c); });
    std::for_each(Tt.begin(), Tt.end(), [&](Lit &c) { _satSolver->addClause(c); });
    while (!lits.empty()&&_satSolver->solve()){
        _status.callSat++;
        model = _satSolver->models();
        lits.clear();
        Tt.clear();
        for (Atom * atom: minimal) {
            if(model[atom->value]){
                lits.push_back({atom, true});
            }else{
                Tt.push_back({atom,true});
            }
        }
        if (lits.empty()){
            // no atom to reduce
            break;
        }
        _satSolver->clear();
        _satSolver->setVars(T.vars());
        _satSolver->setVars(_clauseSet.vars());
        std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
        _satSolver->addClause(lits);
        std::for_each(Y.begin(), Y.end(), [&](Lit &c) { _satSolver->addClause(c); });
        std::for_each(Tt.begin(), Tt.end(), [&](Lit &c) { _satSolver->addClause(c); });

    }
    if(!lits.empty()){
        _status.callSat++;
    }
    this->_model.reserve(T.vars());
    for (int i = 0; i < T.vars(); ++i) {
        _model.push_back(model[i]);
    }
    _satSolver->clear();
    if (!T.getVaried().empty()) {
        _variedSolver->clear();
        _variedSolver->setVars(_orginClauseSet.vars());
        std::for_each(_orginClauseSet.begin(), _orginClauseSet.end(),
                      [&](Clause *c) { _variedSolver->addClause(*c->clause); });
        for (Atom * x: fixed) {
            _variedSolver->addClause(x->value, !model[x->value]);
        }
        for (Atom * x: minimal) {
            _variedSolver->addClause(x->value, !model[x->value]);
        }
    }
    return true;
}
bool CicrLF::Solver::solveVaried() {
    const std::vector<Atom *> &varied = _orginClauseSet.getVaried();
    if (varied.empty()) {
        return false;
    }
    std::vector<Lit> lits;
    for (Atom * x: varied) {
        lits.push_back({x,_model[x->value]==true});
    }
    _variedSolver->addClause(lits);
    bool sat = _variedSolver->solve();
    if (!sat) {
        return false;
    }
    const std::vector<char> &model = _variedSolver->models();
    this->_model.clear();
    this->_model.reserve(_orginClauseSet.vars());
    for (int i = 0; i < _orginClauseSet.vars(); ++i) {
        _model.push_back(model[i]);
    }
    return sat;
}

bool CicrLF::Solver::solve() {
    _status.solveTime = realTime();
//        _status.cpuTime  = cpuTime();
    _status.running= true;
    bool sat = _doSolve();
    _status.solveTime = realTime()-_status.solveTime;
    _status.cpuTime = cpuTime(); //-_status.cpuTime ;
    _status.running= false;
    return sat;
}

bool CicrLF::Solver::solveMore() {
    _status.solveTime = realTime();
//        _status.cpuTime  = cpuTime();
    _status.running= true;
    const std::vector<Atom *> &fixed = _orginClauseSet.getFixed();
    const std::vector<Atom *> &minimal = _orginClauseSet.getMinimal();
    if (solveVaried()){
        _status.solveTime = realTime()-_status.solveTime;
        _status.cpuTime = cpuTime();
        return true;
    }

    std::vector<Lit>* lits=new std::vector<Lit>();
    Clause * clause=new Clause{lits};
    for (Atom * x: fixed) {
        if (_model[x->value]){
            lits->push_back({x, true});
            x->negative.push_back(clause);
        } else{
            lits->push_back({x, false});
            x->positive.push_back(clause);
        }
    }
    for (Atom * x: minimal){
        if(_model[x->value]){
            lits->push_back({x,true});
            x->negative.push_back(clause);
        }
    }
    bool sat = false;
    if(!lits->empty()){
        _clauseSet.addClause(lits);
        sat = _doSolve();
    } else{
        delete lits;
        delete clause;
    }
    _status.solveTime = realTime()-_status.solveTime;
    _status.cpuTime = cpuTime(); //-_status.cpuTime ;
    _status.running= false;
    return sat;
}

CicrLF::Solver::~Solver() {
    delete this->_satSolver;
    delete this->_variedSolver;
}