//
// Created by 谢仲涛 on 2021/2/11.
//

#ifndef CICR_LF_PARSER_H
#define CICR_LF_PARSER_H
#include <list>
#include <vector>
#include <iostream>
#include "../core/CluaseSet.h"

namespace CicrLF {

    class CNFParser {
    private:
        FILE *f;

        inline char readChar() {
            return buff[index];
        }

        inline char readNextChar() {
            ++index;
            if (index == size) {
                size = fread(buff, 1, _max_size, f);
                buff[size] = 0;
                if (size == 0)return 0;
                index = 0;
            }
            return buff[index];
        }

        int size = 0;
        int index = 0;
        const int _max_size = 10 * 1024;
        char *buff = nullptr;
    public:
        CNFParser(const std::string &path) {
            f = fopen(path.c_str(), "rb");
            size = 0;
            index = size;
            buff = new char[_max_size + 1];
            size = fread(buff, 1, _max_size, f);
        }

        ~CNFParser() {
            fclose(f);
        }

        void parse(ClauseSet &set) {
            skipSpace();
            char c = this->readChar();
            while (c) {
                if (c == 'c') {
                    nextLine();
                    skipSpace();
                    c = this->readChar();
                    continue;
                }
                if (c == 'p') {
                    readHeader(set);
                    nextLine();
                    skipSpace();
                    c = this->readChar();
                    continue;
                }
                parseClause(set);
                nextLine();
                skipSpace();
                c = this->readChar();
            }
            set.setInputType(CNF);
        }

    private:
        inline void skipSpace() {
            char c = readChar();
            while (c == ' ')c = readNextChar();
        }

        void nextLine() {
            char c = readChar();
            while (c && c != '\n')c = readNextChar();
            readNextChar();
        }

        inline bool startWith(const char *start) {
            char ch = readChar();
            while (ch && *start) {
                if (ch != *start) {
                    return false;
                }
                ch = readNextChar();
                ++start;
            }
            return start == nullptr;
        }


        inline void readHeader(ClauseSet &set) {
            if (!startWith("p cnf")) {
                return;
            }
            int variables= readInt();
            set.reserve(variables);
            readInt();
        }

        inline void parseClause(ClauseSet &set) {
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
                set.addClause(cls );
            }else{
                delete cls;
                delete lits;
            }
        }

        inline int readInt() {
            skipSpace();
            bool neg = false;
            char c = readChar();
            if (c == '-') {
                neg = true;
                c = readNextChar();
            }
            int value = 0;
            if (c<'0'||c>'9'){
                throw std::exception();
            }
            while (c >= '0' && c <= '9') {
                value = value * 10 + (c - '0');
                c = readNextChar();
            }

            return neg?-value:value;
        }
    };
}

#endif //CICR_LF_PARSER_H
