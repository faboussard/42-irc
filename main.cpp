#include "Server.hpp"

int main() {
    int port = 6667; // Port par défaut pour IRC
    Server ircServer(port);
    ircServer.run();
    return 0;
}
