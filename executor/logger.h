#ifndef LoggerModuleH
#define LoggerModuleH

#include <string>
#include <vector>
#include <fstream>

namespace SnailJudgeExecutor {
    using namespace std;

    class Logger {
        public:
            virtual void log(const string &label, const vector<string> &info) = 0;
    };

    class FileLogger: public Logger {
        public:
            FileLogger(const string &log_file_name);
            virtual ~FileLogger(void);

            void log(const string &label, const vector<string> &info);
        private:
            ofstream fs;
    };
}

#endif
