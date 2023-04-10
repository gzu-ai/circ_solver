//
// Created by xiezt on 2021/11/17.
//

#include "Solver.h"
#include "DnfCompletion.h"
#include "Graph.h"
#include "Eliminate.h"
#include "LoopFormula.h"
namespace CicrLF {
    bool _getLF(ClauseSet& _clauseSet, std::vector<char> &model,
                CicrLF::ClausesType &out,
                CicrLF::ClauseExpander& clauseExpander,
                std::vector<Atom *> &m_p,
                ClauseSet& emlt) {
        CicrLF::DnfLoopFormula loop_formula;
        CicrLF::Graph graph;
        graph.initGraph(_clauseSet,m_p);
        std::vector<Atom *> loop;
        bool result =false;
        while (graph.getLoop(loop,_clauseSet)){
            result=true;
            loop_formula.compute(emlt,loop,out);
        }
        return result;
    }
//    bool _getNextModel(Wrapper *_satSolver,ClauseSet& _clauseSet, std::vector<char> &model,std::vector<Atom *> &m_p,
//                       ClauseExpander& clauseExpander,SolveStatus&status) {
//        const std::vector<Atom *> &minimal = _clauseSet.getMinimal();
//        std::vector<Lit> lits;
//        std::vector<Atom *> el;
//        CicrLF::ClausesType lf;
//        double start =cpuTime();
//        _getLF(_clauseSet,model,lf,clauseExpander,m_p,el);
//        double end =cpuTime();
//        std::cout <<"loop find: " << end -start << std::endl;
//        for (Atom* atom: m_p) {
//            lits.push_back({atom, true});
//        }
//        _satSolver->setVars(clauseExpander.maxVar()+1);
//        std::for_each(lf.begin(), lf.end(), [&](Clause*cls ){_satSolver->addClause(*cls->clause);});
//        _satSolver->addClause(lits);
//        status.callSat++;
//        if(!_satSolver->solve()){
//            return false;
//        }
//        _satSolver->models(_clauseSet.vars(),model);
//        for(Atom* atom:el){
//            model[atom->value] = false;
//        }
//        return true;
//    }
 void compute_m_p(const std::vector<Atom*> &fixed ,const std::vector<Atom*> &minimal ,   std::vector<int> &Y,
    std::vector<Atom*> &p_diff_m,
    std::vector<Atom*> &p_and_m,
    std::vector<Lit> &neg_p_and_m_lits,
    std::vector<char> &model) {
        Y.clear();
        p_diff_m.clear();
        p_and_m.clear();
        neg_p_and_m_lits.clear();
        for (Atom *atom: fixed) {
            unsigned int value = atom->value;
            if (model[value]) {
                Y.push_back(value);
            } else {
                Y.push_back(-value);
            }
        }
        for (Atom *atom: minimal) {
            unsigned int value = atom->value;
            if (model[value]) {
                p_and_m.push_back(atom);
                neg_p_and_m_lits.push_back(Lit{atom, true});
            } else {
                p_diff_m.push_back(atom);
            }
        }
    }

}


void print_cls(CicrLF::ClausesType& lits){
    std::for_each(lits.begin(), lits.end(),[&](CicrLF::Clause *c) {
        std::for_each(c->clause->begin(), c->clause->end(),[&](CicrLF::Lit&lit){
            if (lit.neg){
                std::cout<<"-";
            }
            std::cout<<lit.value->value+1 <<" ";
        });
        std::cout<<std::endl;
    });

}

char*  CicrLF::Solver::description="use max loop";

/// circ_LFC(A, P, Z)

void clear(CicrLF::ClausesType &out){
    for (CicrLF::Clause* it:out) {
        delete it;
    }
    out.clear();
}

bool CicrLF::Solver::_doSolve() {
    ClauseSet  T=_clauseSet;
    ClauseSet exit;
    double start=cpuTime();
    eliminateExist(T,exit);
    double end=cpuTime();
    this->_status.eliminateTime+=(end-start);
    this->_model.clear();
    CicrLF::ClauseExpander clauseExpander(T.vars());
    CicrLF::DnfLoopFormula loop_formula{};

    CicrLF::Completion completion(clauseExpander);
    CicrLF::ClausesType comp= completion.compute(exit,exit.getMinimal());
    _satSolver->setVars(clauseExpander.maxVar() );
    std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
        std::for_each(comp.begin(), comp.end(), [&](Clause *c) {_satSolver->addClause(*c->clause);});
    completion.clear();
    ++_status.callSat;
    if (!_satSolver->solve())return false;
    std::vector<char> model = _satSolver->models();
    const std::vector<Atom*> &fixed = T.getFixed();
    const std::vector<Atom*> &minimal = T.getMinimal();
    std::vector<int> Y;
    std::vector<Atom*> p_diff_m;
    std::vector<Atom*> p_and_m;
    std::vector<Lit> neg_p_and_m_lits;
    compute_m_p(fixed,minimal,Y,p_diff_m,p_and_m,neg_p_and_m_lits,model);
    clauseExpander.setInitVar(T.vars());
    comp= completion.compute(exit,p_and_m);
    while (!p_and_m.empty()) {
        CicrLF::ClausesType lf;
        _getLF(T, model, lf, clauseExpander, p_and_m,exit);
        _satSolver->clear();
        _satSolver->setVars(clauseExpander.maxVar());
        // add tao
        std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
        std::for_each(comp.begin(), comp.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
        // max loop
        std::for_each(lf.begin(), lf.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
        clear(lf);
        std::for_each(Y.begin(), Y.end(), [&](int y) { _satSolver->addClause(abs(y), y < 0); });
        _satSolver->addClause(neg_p_and_m_lits);
        std::for_each(p_diff_m.begin(), p_diff_m.end(), [&](Atom *atom) { _satSolver->addClause(atom->value, true); });
        ++_status.callSat;
        if (!_satSolver->solve())break;
        model = _satSolver->models();
        compute_m_p(fixed,minimal,Y,p_diff_m,p_and_m,neg_p_and_m_lits,model);
    }

    _satSolver->clear();

    this->_model.reserve(T.vars());
    for (int i = 0; i < T.vars(); ++i) {
        _model.push_back(model[i]);
    }
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