#ifndef LOOGER_H
#define LOOGER_H

#include <iostream>
#include <chrono>
#include <ctime> 

using namespace std;

class Logger {

private:
    static void log(const string& level, const string& message) {
        // 2024-08-30 14:20:53 | INFO | COMMAND: ADD pool1 schema1 collection1 1 1 Ivan 123.12
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::time_t end_time = std::chrono::system_clock::to_time_t(end);
        
        const char* format = "%Y-%m-%d %H:%M:%S";
        char formattedTime[90];
        struct tm* timeinfo = localtime(&end_time);
        strftime(formattedTime, sizeof(formattedTime), format, timeinfo);
        
        cout << formattedTime << " | " << level << " | " << message << endl;
    };
    
public:
    static void info(const string& message) {
        log("INFO", message);
    };
    
    static void error(const string& message) {
        log("ERROR", message);
    };
};

#endif