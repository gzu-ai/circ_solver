//
// Created by xiezt on 2022/11/21.
//

#ifndef CICR_LF_LOOPFORMULA_H
#define CICR_LF_LOOPFORMULA_H

#include "TypeDefine.h"
#include "CluaseSet.h"
#include "ClauseExpander.h"

namespace CicrLF {
    class DnfLoopFormula {
    public:

        ClausesType &compute(CicrLF::ClauseSet &eliminated, const std::vector<Atom *> &loop,CicrLF::ClausesType &out);

        inline ClausesType &getCompClauses() { return _clauses; }

        inline void clear() { _clauses.clear(); }

    private:
        ClausesType _clauses;
    };
}

#endif //CICR_LF_LOOPFORMULA_H
