#include <sys/resource.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "logger.h"
#include "monitor.h"

namespace SnailJudgeExecutor {
    using namespace std;

    void setLimit(int type, rlim_t soft_lim, rlim_t hard_lim) {
        struct rlimit limit;
        limit.rlim_cur = (soft_lim > 0 ? soft_lim : RLIM_INFINITY);
        limit.rlim_max = (hard_lim > 0 ? hard_lim : RLIM_INFINITY);
        switch (type) {
            case LIM_CPU:
                setrlimit(RLIMIT_CPU, &limit);
                break;
            case LIM_AS:
                setrlimit(RLIMIT_AS, &limit);
                break;
            case LIM_FSIZE:
                setrlimit(RLIMIT_FSIZE, &limit);
                break;
            default:
                ;
        }
    }

    char *ltostr(int x) {
        char *res = new char [64];
        sprintf(res, "%d", x);
        return res;
    }

    inline unsigned long getOrigRax(struct user_regs_struct *regs) {
        return regs->orig_rax;
    }

    inline unsigned long getRdi(struct user_regs_struct *regs) {
        return regs->rdi;
    }

    void monitor(const char *exe, int time_limit, int memory_limit, int output_limit, const char *input_file, const char *output_file, Logger &logger, vector<int> syscall_cnt, const vector<string> safe_files) {
        pid_t pid = fork();
        if (pid == 0) {
            freopen(input_file, "r", stdin);
            freopen(output_file, "w", stdout);
            setLimit(LIM_CPU, (rlim_t)time_limit, (rlim_t)time_limit);
            setLimit(LIM_AS, (rlim_t)memory_limit, (rlim_t)(memory_limit));
            setLimit(LIM_FSIZE, (rlim_t)output_limit, (rlim_t)output_limit);
            ptrace(PTRACE_TRACEME, 0, NULL, NULL);
            execl(exe, exe, NULL);
        }
        else {
            for (bool normal_exit = true; ; ) {
                int wait_val;
                wait(&wait_val);
                if (!WIFSTOPPED(wait_val)) {
                    if (WIFEXITED(wait_val)) {
                        if (normal_exit) logger.log("Result", {"No_Error"});
                    }
                    else if (normal_exit) {
                        logger.log("Result", {"Memory_Limit_Exceeded"});
                        logger.log("Time", {"0"});
                        logger.log("Memory", {"0"});
                        break;
                    }
                    struct rusage usage;
                    getrusage(RUSAGE_CHILDREN, &usage);
                    logger.log("Time", {ltostr(usage.ru_utime.tv_sec * 1000 + usage.ru_utime.tv_usec / 1000)});
                    logger.log("Memory", {ltostr(usage.ru_maxrss)});
                    break;
                }
                if (WSTOPSIG(wait_val) != SIGTRAP) {
                    int sig = WSTOPSIG(wait_val);
                    if (sig == SIGXCPU) {
                        logger.log("Result", {"Time_Limit_Exceeded"});
                    }
                    else if (sig == SIGXFSZ) {
                        logger.log("Result", {"Output_Limit_Exceeded"});
                    }
                    else {
                        logger.log("Result", {"Runtime_Error"});
                    }
                    normal_exit = false;
                    ptrace(PTRACE_KILL, pid, NULL, NULL);
                    continue;
                }
                struct user_regs_struct regs;
                ptrace(PTRACE_GETREGS, pid, NULL, &regs);
                unsigned long orig_rax = getOrigRax(&regs);
                if (orig_rax >= syscall_cnt.size() || syscall_cnt[orig_rax] == 0) {
                    logger.log("Result", {"Runtime_Error"});
                    normal_exit = false;
                    ptrace(PTRACE_KILL, pid, NULL, NULL);
                    continue;
                }
                -- syscall_cnt[orig_rax];
                if (orig_rax == __NR_open || orig_rax == __NR_access) {
                    unsigned long rdi = getRdi(&regs);
                    long buf[32];
                    for (int i = 0; i < 30; ++ i) {
                        buf[i] = ptrace(PTRACE_PEEKDATA, pid, rdi + (unsigned long)i * sizeof(unsigned long), NULL);
                    }
                    buf[30] = -1;
                    buf[31] = 0;
                    bool is_safe = false;
                    for (const auto &s: safe_files)
                        if (strcmp(s.c_str(), (char*)buf)) {
                            is_safe = true;
                            break;
                        }
                    if (!is_safe) {
                        logger.log("Result", {"Runtime_Error"});
                        normal_exit = false;
                        ptrace(PTRACE_KILL, pid, NULL, NULL);
                        continue;
                    }
                }
                ptrace(PTRACE_SYSCALL, pid, NULL, NULL);
            }
        }
    }
}
