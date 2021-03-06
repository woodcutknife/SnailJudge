#include <cstdlib>
#include <vector>
#include <string>
#include "logger.h"
#include "compiler.h"

namespace SnailJudgeExecutor {
    using namespace std;

    int compile(const string &lang, const string &command, const string &file_name, Logger &logger) {
        string compile_command = command + " " + file_name;
        if (lang == "pascal")
            compile_command += " 1>.compile_info";
        else
            compile_command += " 2>.compile_info";
        int return_code = system(compile_command.c_str());
        vector<string> info;
        ifstream tmp(".compile_info", ios_base::in);
        for (string s; getline(tmp, s); info.push_back(s));
        tmp.close();
        system("rm .compile_info");
        logger.log("Compile_Info", info);
        system("rm Main.o");
        return return_code;
    }
}
