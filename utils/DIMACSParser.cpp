//
// Created by xiezt on 2021/11/11.
//

#include <zlib.h>
#include "DIMACSParser.h"
namespace CicrLF {
    void DIMACSParser::parse(ClauseSet &clausesSet) {
        readHeader(clausesSet);
        clausesSet.setInputType(CNF);
        this->skip();
        while (!isEOF()) {
            this->readClause(clausesSet);
            this->skip();
        }
    }

    DIMACSParser::DIMACSParser(const char *path) {
        in = gzopen(path, "rb");
        pos = 0;
        size = gzread(in, buff, BUFF_SIZE);
        buff[size] = 0;
    }

    void DIMACSParser::readHeader(ClauseSet &set) {

        while (this->skip() == 'c') {
            skipLine();
        }
        if (!match("p cnf")) {
            return;
        }
        int variables= readInt();
        set.reserve(variables);
        readInt();
    }

    char DIMACSParser::readChar() {
        return buff[pos];
    }

    char DIMACSParser::nextChar() {
        pos++;
        if (pos >= size) {
            if (size < BUFF_SIZE)return 0;
            pos = 0;
            size = gzread(in, buff, BUFF_SIZE);
            buff[size] = 0;
        }
        return buff[pos];
    }

    char DIMACSParser::skip() {
        char c = readChar();
        while (c == '\n' || c == ' ' ||c=='\r')
            c = nextChar();
        return c;

    }

    int DIMACSParser::readInt() {
        char c = this->skip();
        int sig = 1;
        int val = 0;
        while ((c < ':' && c > '/') || c == '-') {
            if (c == '-') {
                sig *= -1;
            } else {
                val = val * 10 + (c - '0');
            }
            c = nextChar();
        }
        return val * sig;
    }

    int DIMACSParser::readClause(ClauseSet &set) {
        int val;
        std::vector<Lit> * lits= new std::vector<Lit>;
        Clause* cls=new Clause{lits};
        while ((val = readInt())) {
            Lit  lit = set.makeLit((unsigned int) abs(val)-1, val < 0);
            lits->push_back(lit);
            if (lit.neg){
                lit.value->negative.push_back(cls);
            }else{
                lit.value->positive.push_back(cls);
            }
        }
        if (!lits->empty()) {
            set.addClause(lits);
        }else{
            delete cls;
            delete lits;
        }
        return 0;
    }

    bool DIMACSParser::isEOF() {
        if (pos < size)return false;

        if (size < BUFF_SIZE) {
            return true;
        }
//        nextChar();

        return false;
    }

    bool DIMACSParser::match(const char *str) {
        while (*str != 0 && !this->isEOF()) {
            if (*str != this->readChar())return false;
            str++;
            this->nextChar();
        }
        return true;
    }

    void DIMACSParser::skipLine() {
        while (readChar() != '\n') nextChar();
    }
}


