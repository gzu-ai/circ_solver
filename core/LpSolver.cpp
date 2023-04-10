//
// Created by blockdata on 2022/2/9.
//

#include "Solver.h"
#include "Completion.h"
#include "Graph.h"
#include "utils/SetUtils.h"

//void printClauses(CicrLF::ClausesType &clauses){
//    printf("----------------\n");
//    std::for_each(clauses.begin(), clauses.end(),[&](CicrLF::Clause& lits){
//        std::for_each(lits.begin(), lits.end(),[&](CicrLF::Lit& lit){
//           printf("%s%d ",lit.neg?"-":"",lit.value+1);
//        });
//        if (!lits.empty())
//        printf("\n");
//    } );
//    fflush(stdout);
//}
//void printModel(std::vector<char>& model){
//    for (int i=0;i<model.size();i++){
//        if(model[i]){
//            printf("%d ",i+1);
//        }
//    }
//    printf("\n");
//    fflush(stdout);
//}
namespace CicrLF {
    bool _getLF(ClauseSet& _clauseSet, std::vector<char> &model,
                CicrLF::ClausesType &out,CicrLF::ClauseExpander& clauseExpander,std::vector<Atom *> &m_p,std::vector<Atom *> &el) {
        const std::vector<Atom *> &minimal = _clauseSet.getMinimal();

        CicrLF::Graph graph;
        graph.initGraph(_clauseSet,m_p);
        std::vector<Atom *> loop;
        bool result =false;
        while (graph.getLoop(loop,_clauseSet)){
            result=true;
            ClausesType tmp;
            auto success = eliminatedClauses(_clauseSet,loop,tmp);
            if (!success) continue;
            if (!tmp.empty()){
                clauseExpander.imply(loop,tmp, out);
            }else{
                for (Atom * atom:loop) {
                    model[atom->value]=0;
                    el.push_back(atom);
                }
            }
        }
        return result;
    }
    bool _getNextModel(Wrapper *_satSolver,ClauseSet& _clauseSet, std::vector<char> &model,std::vector<Atom *> &m_p,
                       ClauseExpander& clauseExpander,SolveStatus&status) {
        const std::vector<Atom *> &minimal = _clauseSet.getMinimal();
        std::vector<Lit> lits;
        std::vector<Atom *> el;
        CicrLF::ClausesType lf;
            double start =cpuTime();
            _getLF(_clauseSet,model,lf,clauseExpander,m_p,el);
            double end =cpuTime();
            std::cout <<"loop find: " << end -start << std::endl;
        for (Atom* atom: m_p) {
            lits.push_back({atom, true});
        }
        _satSolver->setVars(clauseExpander.maxVar()+1);
       std::for_each(lf.begin(), lf.end(), [&](Clause*cls ){_satSolver->addClause(*cls->clause);});
       _satSolver->addClause(lits);
        status.callSat++;
        if(!_satSolver->solve()){
            return false;
        }
        _satSolver->models(_clauseSet.vars(),model);
        for(Atom* atom:el){
            model[atom->value] = false;
        }
        return true;
    }

}


bool CicrLF::Solver::_doSolve() {
    ClauseSet  T=_clauseSet;
    this->_model.clear();
    CicrLF::ClauseExpander clauseExpander(T.vars());

    CicrLF::Completion completion(clauseExpander);
//    CicrLF::ClausesType comp;// = completion.compute(T,T.getMinimal());
    _satSolver->setVars(clauseExpander.maxVar() );
    std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
//    std::for_each(comp.begin(), comp.end(), [&](Clause *c) {_satSolver->addClause(*c->clause);});
    ++_status.callSat;
    if (!_satSolver->solve())return false;
    std::vector<char> model;
    _satSolver->models(T.vars(), model);
    std::vector<char> store(T.vars(), false);
    const std::vector<Atom *> &fixed = T.getFixed();
    const std::vector<Atom *> &minimal = T.getMinimal();
    std::vector<Atom*> beT;
    std::vector<Atom*> beF;
    std::vector<Atom*> atoms; // M 交 P
    for (Atom * atom: fixed) {
        unsigned  int value =atom->value;
        store[value] = model[value];
        if (model[value]){
            beT.push_back(atom);
        } else{
            beF.push_back(atom);
        }
    }
    for (Atom * atom: minimal) {
        unsigned  int value =atom->value;
        if (model[value]){
            atoms.push_back(atom);
        } else{
            beF.push_back(atom);
        }
    }
   redact(T,beT, beF);
   clauseExpander.setInitVar(T.vars());
//   comp = completion.compute(T,atoms);
   _satSolver->clear();
   _satSolver->setVars(clauseExpander.maxVar() );
   std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
//   std::for_each(comp.begin(), comp.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
   beT.clear();
   while (!atoms.empty()&&_getNextModel(_satSolver, T, model,atoms, clauseExpander,_status)) {
       _satSolver->clear();
       atoms.clear();
       for (Atom * atom: minimal) {
           if(model[atom->value]){
               atoms.push_back(atom);
           }else{
            beF.push_back(atom);
           }
       }
       if (atoms.empty()){
           break;
       }
       clauseExpander.setInitVar(_orginClauseSet.vars());
       redact(T,beT, beF);
//       comp = completion.compute(T,atoms);
       _satSolver->setVars(clauseExpander.maxVar() );
       std::for_each(T.begin(), T.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
//       std::for_each(comp.begin(), comp.end(), [&](Clause *c) { _satSolver->addClause(*c->clause); });
   }
    this->_model.reserve(T.vars());
    for (int i = 0; i < T.vars(); ++i) {
        _model.push_back(model[i]);
    }
    for (Atom * atom: fixed) {
        unsigned  int value=atom->value;
        _model[value] = store[value];
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


