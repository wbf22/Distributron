#pragma once


#include <string>
#include <unordered_map>


using namespace std;

struct Settings {
    inline static int NUM_THREADS = 10;
    inline static int RATE_LIMIT = 200;
    inline static double THROTTLE_RATE = 1.5;
    inline static int MAX_REQUEST_SIZE_BYTES = 65536;

    
    static void set(unordered_map<string, string> settings) {
        Settings::NUM_THREADS = get<int>("NUM_THREADS", settings, Settings::NUM_THREADS);
        Settings::RATE_LIMIT = get<int>("RATE_LIMIT", settings, Settings::RATE_LIMIT);
        Settings::THROTTLE_RATE = get<double>("RATE_LIMIT", settings, Settings::THROTTLE_RATE);
        Settings::MAX_REQUEST_SIZE_BYTES = get<double>("MAX_REQUEST_SIZE_BYTES", settings, Settings::MAX_REQUEST_SIZE_BYTES);
    }

    template <typename T>
    static T get(const string& key, unordered_map<string, string> settings, T default_value) {
        string value = settings.find(key) != settings.end() ? settings[key] : "";
        if (value != "") {
            if constexpr (std::is_same<T, int>::value) {
                return std::stoi(value);
            } 
            else if constexpr (std::is_same<T, bool>::value) {
                return value == "true" || value == "1";
            } 
            else if constexpr (std::is_same<T, string>::value) {
                return value;
            }   
            else if constexpr (std::is_same<T, double>::value) {
                return std::stod(value);
            }
        }

        return default_value;
    }
};