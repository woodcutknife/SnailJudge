#ifndef MonitorModuleH
#define MonitorModuleH

#include <sys/resource.h>
#include <vector>
#include "logger.h"

#define LIM_CPU 0
#define LIM_AS 1
#define LIM_FSIZE 2

namespace SnailJudgeExecutor {
    using namespace std;

    void setLimit(int type, rlim_t soft_lim, rlim_t hard_lim);
    void monitor(const char *exe, int time_limit, int memory_limit, int output_limit, const char *input_file, const char *output_file, Logger &logger, vector<int> syscall_cnt, const vector<string> safe_files);
}

#endif
