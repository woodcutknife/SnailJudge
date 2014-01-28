#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include <string>
#include <fstream>
#include "logger.h"
#include "compiler.h"
#include "monitor.h"

using namespace std;

inline string getKey(const string &str, int start = 0) {
    auto pos = str.find('=');
    return str.substr(start, pos - start);
}

inline string getValue(const string &str) {
    auto pos = str.find('=');
    return str.substr(pos + 1);
}

inline long str2l(const char *str) {
    char *ptr;
    return strtol(str, &ptr, 10);
}

// Usage: ./executor FILENAME LANGUAGE 
//                   [ --time_limit=1000 ] [ --memory_limit=256 ] [ --output_limit=10 ] 
//                   [ --input_file=_.in ] [ --output_file=_.out ]
//                   [ --log_file=log ]
int main(int argc, char* argv[]) {
    string file_name = argv[1];
    string lang = argv[2];
    unordered_map<string, string> args;
    args["time_limit"] = "1000";
    args["memory_limit"] = "256";
    args["output_limit"] = "10";
    args["input_file"] = "_.in";
    args["output_file"] = "_.out";
    args["log_file"] = "log";
    for (int i = 3; i < argc; ++ i)
        args[getKey(argv[i], 2)] = getValue(argv[i]);

    unordered_map<string, string> compile_commands;
    ifstream compile_ini;
    compile_ini.open("compiler.ini", ios_base::in);
    for (string s; getline(compile_ini, s); compile_commands[getKey(s)] = getValue(s));
    compile_ini.close();

    SnailJudgeExecutor::FileLogger logger(args["log_file"]);

    int compile_return_code = SnailJudgeExecutor::compile(
            compile_commands[lang], file_name, logger);
    if (compile_return_code != 0) {
        logger.log("Result", {"Compile_Error"});
        return 1;
    }

    int time_limit = str2l(args["time_limit"].c_str());
    int memory_limit = str2l(args["memory_limit"].c_str());
    int output_limit = str2l(args["output_limit"].c_str());

    vector<int> syscall_cnt;
    ifstream syscall_ini;
    syscall_ini.open("syscall_ini/" + lang + ".ini", ios_base::in);
    int size;
    syscall_ini >> size;
    syscall_cnt.resize(size, 0);
    for (string s; syscall_ini >> s; syscall_cnt[str2l(getKey(s).c_str())] = str2l(getValue(s).c_str()));
    syscall_ini.close();

    vector<string> safe_files;
    ifstream safe_files_ini;
    safe_files_ini.open("safe_files_ini/" + lang + ".ini", ios_base::in);
    for (string s; safe_files_ini >> s; safe_files.push_back(s));
    safe_files_ini.close();

    monitor(
            "./Main", 
            time_limit / 1000 + 1,
            memory_limit * 1024 * 1024, 
            output_limit * 1024 * 1024,
            args["input_file"].c_str(),
            args["output_file"].c_str(),
            logger,
            syscall_cnt,
            safe_files
           );

    system("rm Main Main.o");

    return 0;
}
