#include "include/Server.hpp"
#include <cstdlib>
#include <csignal>

IRCServer* g_server = NULL;

void signalHandler(int signum) {
    std::cout << "\n[Server] Caught signal " << signum << ", shutting down..." << std::endl;
    if (g_server) {
        delete g_server;
        g_server = NULL;
    }
    exit(0);
}

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << "Use " << av[0] << " <password> <port>" << std::endl;
        return 1;
    }
    std::string password = av[1];
    int port = atoi(av[2]);  
    g_server = new IRCServer(password, port);
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGTSTP, signalHandler);
	signal(SIGQUIT, signalHandler);
    if (!g_server->setupServer()) {
        delete g_server;
        return 1;
    }
    g_server->createBot();
    g_server->run();
    delete g_server;
    return 0;
}
