#pragma once


#include <unordered_map>
#include <vector>
#include "../util/Util.hpp"
#include "../util/Settings.hpp"
#include <cstdint>



using namespace std;


struct RateLimit {

    inline static int MAX_KEEP_RECORD_TIME = 60000 * 5; // 5 minutes
    inline static int MAX_RATE_LIMIT = 2000; // 2 seconds

    inline static unordered_map<string, int> ip_to_last_request_time;
    inline static unordered_map<string, int> ip_to_rate_limit;
    inline static vector<string> ip_addresses;
    inline static int index_cntr = 0;


    int64_t time_to_wait(const string& ip_address) {

        int64_t current_time = Util::time_ms();

        // do maintenance
        int iterations = RateLimit::ip_addresses.size() / 100;
        for (int i = 0; i < iterations; ++i) {
            string& ip_address = RateLimit::ip_addresses[index_cntr];
            if (current_time - RateLimit::ip_to_last_request_time[ip_address] > MAX_KEEP_RECORD_TIME) {
                RateLimit::ip_to_last_request_time.erase(ip_address);
                RateLimit::ip_to_rate_limit.erase(ip_address);

                // remove the IP address from the vector
                RateLimit::ip_addresses[index_cntr] = RateLimit::ip_addresses.back();
                RateLimit::ip_addresses.pop_back();
            }
            ++index_cntr;
        }

        // return immediately if new client
        if (RateLimit::ip_to_last_request_time.find(ip_address) == RateLimit::ip_to_last_request_time.end()) {
            RateLimit::ip_to_last_request_time[ip_address] = current_time;
            RateLimit::ip_to_rate_limit[ip_address] = Settings::RATE_LIMIT;
            RateLimit::ip_addresses.push_back(ip_address);
            return 0;
        }



        // if the last request was made to recent set the time to wait
        int64_t elapsed_time = current_time - RateLimit::ip_to_last_request_time[ip_address];
        int64_t time_to_wait = 0;
        if (elapsed_time < RateLimit::ip_to_rate_limit[ip_address]) {
            RateLimit::ip_to_rate_limit[ip_address] *= Settings::THROTTLE_RATE; // increase the rate limit
            if (RateLimit::ip_to_rate_limit[ip_address] > MAX_RATE_LIMIT) {
                RateLimit::ip_to_rate_limit[ip_address] = -1; // set to auto reject
            }
            time_to_wait = RateLimit::ip_to_rate_limit[ip_address];
        }
        else {
            RateLimit::ip_to_rate_limit[ip_address] /= Settings::THROTTLE_RATE; // decrease the rate limit
            time_to_wait = RateLimit::ip_to_rate_limit[ip_address];
        }

        // update the last request time
        RateLimit::ip_to_last_request_time[ip_address] = current_time;

        return time_to_wait;
    }



};

