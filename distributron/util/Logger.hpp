#pragma once


#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>


#define L_RED "\033[1;31m"
#define L_ORANGE "\033[1;33m"
#define L_GREEN "\033[1;32m"
#define L_BLUE "\033[1;34m"
#define L_YELLOW "\033[1;33m"
#define L_PURPLE "\033[1;35m"
#define L_RESET "\033[0m"
#define L_LIGHT_GRAY "\033[1;37m"
#define L_DARK_GRAY "\033[1;30m"
#define L_GRAY "\033[0;37m"

using namespace std;

enum LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR
};


struct Logger {
    static inline LogLevel LEVEL = INFO;
    static inline bool PRINT_TIME = true;

    static string time() {
        // Get the current time
        auto now = chrono::system_clock::now();
        auto in_time_t = chrono::system_clock::to_time_t(now);
        auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // Convert to tm structure
        std::tm buf;
        localtime_r(&in_time_t, &buf);

        // Format the time into a string
        std::ostringstream oss;
        oss << L_GRAY << '[' << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
        oss << ']';
        return oss.str();
    }

    static void trace(const string message) {
        if (LEVEL <= TRACE) {
            cerr << L_PURPLE << "TRACE: ";
            if (PRINT_TIME) cerr << time() << " ";
            cerr << L_RESET << message  << endl;
        }
    }

    static void debug(const string message) {
        if (LEVEL <= DEBUG) {
            cerr << L_GREEN << "DEBUG: ";
            if (PRINT_TIME) cerr << time() << " ";
            cerr << L_RESET << message  << endl;
        }
    }

    static void info(const string message) {
        if (LEVEL <= INFO) {
            cerr << L_BLUE << "INFO: ";
            if (PRINT_TIME) cerr << time() << " ";
            cerr << L_RESET << message  << endl;
        }
    }

    static void warn(const string message) {
        if (LEVEL <= WARN) {
            cerr << L_ORANGE << "WARN: ";
            if (PRINT_TIME) cerr << time() << " ";
            cerr << L_RESET << message  << endl;
        }
    }

    static void error(const string message) {
        if (LEVEL <= ERROR) {
            cerr << L_RED << "ERROR: ";
            if (PRINT_TIME) cerr << time() << " ";
            cerr << L_RESET << message  << endl;
        }
    }
};

