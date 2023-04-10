#include <unordered_map>
#include "Eliminate.h"
#include "config.h"
// return \neg \exit z A(P,z)
void eliminateExist(CicrLF::ClauseSet& set,CicrLF::ClauseSet& out){
    auto *_satSolver=new Wrapper();
    out.copySimple(set);
    while(true){
        _satSolver->setVars(set.vars());
        std::for_each(set.begin(),set.end(),[&](CicrLF:: Clause *c) { _satSolver->addClause(*c->clause); });
        std::for_each(out.begin(),out.end(),[&](CicrLF:: Clause *c) { _satSolver->addClause(*c->clause); });
        if(!_satSolver->solve())break;
        auto&  models= _satSolver->models();
        auto * lits=new std::vector<CicrLF::Lit>();
        auto* cls=new CicrLF::Clause{lits};
        for (int i = 0; i <set.vars(); ++i) {
            CicrLF::Atom* atom= set.getAtom(i);
            if (atom->type==CicrLF::AtomType::VARIABLE){
                continue;
            }
            auto lit= out.makeLit(i,models[i]);
            if (lit.neg){
                lit.value->negative.push_back(cls);
            }else{
                lit.value->positive.push_back(cls);
            }
            lits->push_back(lit);
        }
        _satSolver->clear();
        out.addClause(cls);
        printf("size: %d\n",out.size());
    }
    printf("oooooooooooooooooook\n");
}
