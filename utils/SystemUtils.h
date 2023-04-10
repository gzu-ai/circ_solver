//
// Created by 谢仲涛 on 2021/2/20.
//

#ifndef CICR_LF_SYSTEMUTILS_H
#define CICR_LF_SYSTEMUTILS_H

#include <malloc.h>
#include <sys/resource.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>
#include <ctime>

namespace CicrLF{
     double memUsed();

    double cpuTime();
    double realTime();
    void   limitMemory(rlim_t max_mem);
    void limitTime(rlim_t lim);
    static void limitResource(rlim_t lim,int res,const char *);
}


static inline int memReadStat(int field)
{
    char  name[256];
    pid_t pid = getpid();
    int   value;

    sprintf(name, "/proc/%d/statm", pid);
    FILE* in = fopen(name, "rb");
    if (in == NULL) return 0;

    for (; field >= 0; field--)
        if (fscanf(in, "%d", &value) != 1)
            printf("ERROR! Failed to parse memory statistics from \"/proc\".\n"), exit(1);
    fclose(in);
    return value;
}

inline double CicrLF::memUsed() {
    return (double)memReadStat(0) * (double)getpagesize() / (1024*1024);
}
inline double CicrLF::cpuTime() {
    struct rusage ru{};
    getrusage(RUSAGE_SELF, &ru);
    return (double)ru.ru_utime.tv_sec + (double)ru.ru_utime.tv_usec / 1000000;
//    double t= (double)clock()/CLOCKS_PER_SEC;
//      return t;
}

inline void CicrLF::limitMemory(rlim_t max_mem) {
    limitResource(max_mem*1024*1024,RLIMIT_AS,"Warning! Cann't limit memory");
}

inline void CicrLF::limitResource(rlim_t lim, int res,const char * msg) {
    if (lim) {
        rlimit rl;
        getrlimit(res, &rl);
        if (rl.rlim_max == RLIM_INFINITY || lim < rl.rlim_max) {
            rl.rlim_cur = lim;
            if(setrlimit(res, &rl)==-1)
                printf("%s\n", msg);
        }
    }
}

inline void CicrLF::limitTime(rlim_t lim) {
    limitResource(lim,RLIMIT_CPU,"Warning! Cann't limit cpu time");
}

inline double CicrLF::realTime() {
    timeval now{};
    return gettimeofday(&now, nullptr) == 0
           ? static_cast<double>(now.tv_sec + now.tv_usec/1000000.0 )
           : 0.0;
}



#endif //CICR_LF_SYSTEMUTILS_H
