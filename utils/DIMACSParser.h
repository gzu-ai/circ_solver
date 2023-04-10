//
// Created by xiezt on 2021/11/11.
//

#ifndef CIRC_DIMACSPARSER_H
#define CIRC_DIMACSPARSER_H
#define BUFF_SIZE 1024*1024
#include "core/CluaseSet.h"

#include <zlib.h>
namespace CicrLF {
    class DIMACSParser {
    public:
        explicit DIMACSParser(const char *path);

        void parse(ClauseSet &clausesSet);

    private:
        void readHeader(ClauseSet &set);

        inline char readChar();

        inline char nextChar();

        inline bool isEOF();

        inline int readInt();

        inline int readClause(ClauseSet &set);

        inline bool match(const char *str);

        inline char skip();

        inline void skipLine();

        gzFile in;
        int size;
        int pos;
        char buff[BUFF_SIZE+1]{};
    };
}

#endif //CIRC_DIMACSPARSER_H
