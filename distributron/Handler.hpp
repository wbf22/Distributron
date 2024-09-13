#pragma once


#include <memory>
#include <string>
#include "Server.hpp"


using namespace std;    


struct Handler {
    virtual string handle_request(shared_ptr<Request> request) = 0;
    virtual ~Handler() = default;
};