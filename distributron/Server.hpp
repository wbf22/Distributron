#pragma once


#include <mutex>
#include <iostream>
#include <thread>
#include "util/Logger.hpp"
#include "sockets/WindowsInit.hpp"
#include "sockets/TCPSocket.hpp"
#include "rate-limit/RateLimit.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include <memory>
#include "Handler.hpp"




using namespace std;


struct Request {
    int port;
    string ip_address;
    SOCKET socket;
    int64_t scheduled_time;

    unordered_map<string, string> headers;
    string& body;

    Request(int port, const string& ip_address, SOCKET socket, int64_t scheduled_time)
        : port(port), ip_address(ip_address), socket(socket), scheduled_time(scheduled_time) {}
    
};


struct Server {

    bool running = true;

    // threads
    vector<thread> worker_threads;

    // sockets
    mutex socket_mtx;
    TCPSocket socket;

    // rate limiting
    mutex rate_limit_mtx; // mutex for thread safety
    RateLimit rate_limit;

    // request queue
    mutex queue_mtx;
    queue<shared_ptr<Request>> client_requests;

    // handlers
    unordered_map<string, Handler> handlers;


    Server() {

        // set up worker threads
        for (int i = 0; i < Settings::NUM_THREADS; i++)
        {
            thread t([this]() {
                Logger::trace("Worker thread started");
                this->handle_request();
            });

            worker_threads.push_back(std::move(t));
        }
    }

    ~Server() {
        for (auto& t : worker_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        WindowsInit::cleanup();
    }

    void run() {

        Logger::info("Started server");
        WindowsInit::init();

        TCPSocket server_socket = TCPSocket();
        server_socket.server_init(8080);

        
        while(this->running) {
            try {
                // accept incoming connections
                sockaddr_in client;
                SOCKET client_socket = this->accept(client);
                Logger::trace("TCP connect socket: " + to_string(client_socket));
                if (client_socket == -1) {
                    // sleep a bit if nothing recieved
                    this_thread::sleep_for(chrono::milliseconds(50));
                }


                // give to worker threads
                string ip_address;
                int port;
                Server::get_ipaddr_port(client, ip_address, port);
                int64_t wait_time = this->get_rate_limit_time(ip_address);
                if (wait_time != -1) {
                    this->enqueue(std::make_shared<Request>(port, ip_address, socket, Util::time_ms + wait_time));
                }
            }
            catch(exception& e) {
                // debug so hackers can slow us down with weird junk
                Logger::debug("Error in tcp thread");
                Logger::debug(e.what());
            }
        }
    }

    void handle_request() {

        while (this->running) {
            try {
                // pop off a request
                shared_ptr<Request> request = this->get_request();

                // process
                if (request != nullptr) {
                    int64_t current_time = Util::time_ms();

                    // rate limit
                    if (current_time < request->scheduled_time) {
                        // requeue and skip processing for now
                        this->enqueue(request);
                        return; 
                    }
                    
                    // recieve from connection
                    char* buffer = new char[Settings::MAX_REQUEST_SIZE_BYTES];
                    int bytes_read = -1;
                    this->recieve(request->socket, buffer, Settings::MAX_REQUEST_SIZE_BYTES, bytes_read);
                    if (bytes_read == -1) {
                        return;
                    }

                    // parse request
                    this->parse(buffer, bytes_read, request);
                    delete[] buffer;

                    // invoke handler
                    try {
                        string& handler_key = request->headers["REQUEST_TYPE"];
                        string response = this->handlers[handler_key].handle_request(request);

                        // send response
                        this->send(request->socket, response.c_str(), response.length());
                    }
                    catch (exception& e) {
                        // XXX: send back valid http response
                        string response = "{\"error\": \"" + string(e.what()) + "\"}";
                        this->send(request->socket, response.c_str(), response.length());
                    }


                    // close connection
                    // XXX: respect keep alive here and manage that
                    this->close(request->socket);

                }
                else {
                    this_thread::sleep_for(chrono::milliseconds(200)); // sleep for a bit if no requests
                }

            }
            catch (exception& e) {
                Logger::debug("Error in worker thread: " + string(e.what()));
            }

        }

    }



    // RATE LIMIT

    int64_t get_rate_limit_time(const string &ip_address) {
        lock_guard<mutex> lock(rate_limit_mtx);
        return rate_limit.time_to_wait(ip_address);
    }


    // REQUESTS

    shared_ptr<Request> get_request() {
        lock_guard<mutex> lock(queue_mtx);
        if (client_requests.empty()) return nullptr;
        shared_ptr<Request> request = this->client_requests.front();
        this->client_requests.pop();

        return request;
    }

    void enqueue(shared_ptr<Request> request) {
        lock_guard<mutex> lock(queue_mtx);
        this->client_requests.push(request);
    }

    void parse(const char* buffer, int bytes_read, shared_ptr<Request> request) {
        // parse the request headers and body here
    }



    // SOCKETS

    static void get_ipaddr_port(const sockaddr_in& client, string& ipaddr, int& port) {
        // get ip address
        char ipStr[INET6_ADDRSTRLEN];
        if (client.sin_family == AF_INET) {
            // Handle IPv4
            inet_ntop(AF_INET, &((struct sockaddr_in *)&client)->sin_addr, ipStr, sizeof(ipStr));
        } else if (client.sin_family == AF_INET6) {
            // Handle IPv6
            inet_ntop(AF_INET6, &((struct sockaddr_in6 *)&client)->sin6_addr, ipStr, sizeof(ipStr));
        }
        ipaddr = string(ipStr);

        // get port
        port = ntohs(client.sin_port); // Convert network byte order to host byte order
    }

    SOCKET accept(sockaddr_in &client_addr) {
        lock_guard<mutex> lock(socket_mtx);
        return socket.acceptConnection(client_addr);
    }

    void recieve(SOCKET client_socket, char* buffer, int buffer_length, int& bytes_read) {
        lock_guard<mutex> lock(socket_mtx);
        bytes_read = socket.receive(client_socket, buffer, buffer_length);
    }

    void send(SOCKET client_socket, const char* message, int message_length) {
        lock_guard<mutex> lock(socket_mtx);
        socket.send(client_socket, message, message_length);
    }

    void close(SOCKET client_socket) {
        lock_guard<mutex> lock(socket_mtx);
        socket.close_connection(client_socket);
    }


};

