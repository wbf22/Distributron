#pragma once


#include <chrono>
#include <cstdint>


using namespace std;

struct Util {


    static int64_t time_ms()
    {
        auto now = chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        int64_t milliseconds = chrono::duration_cast<chrono::milliseconds>(duration).count();
        return milliseconds;
    }
};
