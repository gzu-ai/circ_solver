//
// Created by 谢仲涛 on 2021/3/6.
//

#ifndef CICR_LF_TYPEDEFINE_H
#define CICR_LF_TYPEDEFINE_H
#include <utility>
#include <vector>
#include <algorithm>
namespace CicrLF {
    enum InputType {
        CNF, LP
    };
    enum AtomType {
        UNKOWN, FIXED, MINIMAL, VARIABLE
    };
    struct Clause;
    struct Atom {
        unsigned int value;
        AtomType type;
        bool flag;
        std::vector<Clause*> positive;
        std::vector<Clause*> negative;
    };

    inline Atom *makeAtom(unsigned int value, AtomType type = UNKOWN,std::vector<Clause*> positive={},std::vector<Clause*> negative={}) {
        return new Atom{value, type, false,std::move(positive),std::move(negative)};
    }

    struct Lit {
        Atom *value;
        bool neg;

        bool operator==(const Lit &lit) const { return value == lit.value && neg == lit.neg; }

        bool operator==(unsigned int lit) const { return value->value == lit; }

        bool operator!=(const Lit &lit) const { return value != lit.value || neg == lit.neg; }

        bool operator!=(unsigned int lit) const { return value->value != lit; }

        bool operator<(const Lit &lit) const {
            if (value == lit.value) return !neg && lit.neg;
            return value < lit.value;
        }

        bool operator<(int lit) const { return value->value < lit; }

        bool operator>(const Lit &lit) const {
            if (value == lit.value) return neg && !lit.neg;
            return value > lit.value;
        }

        bool operator>(unsigned int lit) const { return value->value > lit; }

        Lit operator!() { return {value, !neg}; }
    };

    struct LitIntCompare {
        inline bool operator()(const Lit &lit, const unsigned int &i) {
            return lit.value->value < i;
        }

        inline bool operator()(const unsigned int &i, const Lit &lit) {
            return i < lit.value->value;
        }
    };

    struct lbool {
    private:
        char data;
    private:
        bool unknown() const { return data == 0; }

        bool sat() const { return data == 1; }

        bool unsat() const { return data == -1; }
    };

    struct Clause {
        std::vector <Lit> *clause;
        bool emt;
    };


    typedef std::vector <Clause*> ClausesType;
}
#endif //CICR_LF_TYPEDEFINE_H
