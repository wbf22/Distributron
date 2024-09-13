

#include <iostream>
#include "Server.hpp"


using namespace std;


Server server = Server();


void signalHandler(int signum) {


    server.~Server(); // Call the destructor to clean up resources

    std::cout << "Interrupt signal (" << signum << ") received.\n";
    // Cleanup and close up stuff here
    // Terminate program
    exit(signum);
}


int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);



    server.run();
}

