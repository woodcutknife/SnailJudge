#ifndef CompilerModuleH
#define CompilerModuleH

#include <string>
#include "logger.h"

namespace SnailJudgeExecutor {
    using namespace std;

    int compile(const string &lang, const string &command, const string &file_name, Logger &logger);
}

#endif
