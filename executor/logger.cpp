#include <string>
#include <vector>
#include <fstream>
#include "logger.h"

namespace SnailJudgeExecutor {
    using namespace std;

    FileLogger::FileLogger(const string &log_file_name) {
        fs.open(log_file_name.c_str(), ios_base::out);
    }

    FileLogger::~FileLogger(void) {
        fs.close();
    }

    void FileLogger::log(const string &label, const vector<string> &info) {
        fs << label << ": " << info.size() << endl;
        for (const auto &s: info) fs << s << endl;
    }
}
