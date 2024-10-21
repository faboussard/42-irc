#include "Server.hpp"

Server::Server(int port) {
    // Créer un socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Échec de la création du socket." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Configurer l'adresse du serveur
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Lier le socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        std::cerr << "Échec de la liaison." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Écouter les connexions entrantes
    if (listen(server_fd, 3) < 0) {
        std::cerr << "Échec de l'écoute." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Serveur en écoute sur le port " << port << std::endl;
    
    // Ajouter le socket du serveur à la liste des descripteurs à surveiller
    struct pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN;
    pfd.revents = 0;
    poll_fds.push_back(pfd);
}

void Server::run() {
    while (true) {
        // Utiliser poll pour surveiller les descripteurs
        int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
        if (poll_count < 0) {
            std::cerr << "Erreur lors de l'utilisation de poll." << std::endl;
            continue;
        }

        // Vérifier les descripteurs surveillés
        for (size_t i = 0; i < poll_fds.size(); ++i) {
            if (poll_fds[i].revents & POLLIN) {
                if (poll_fds[i].fd == server_fd) {
                    // Nouvelle connexion entrante
                    int client_socket = accept(server_fd, NULL, NULL);
                    if (client_socket < 0) {
                        std::cerr << "Échec de l'acceptation." << std::endl;
                    } else {
                        std::cout << "Nouvelle connexion acceptée." << std::endl;
                        struct pollfd client_pfd;
                        client_pfd.fd = client_socket;
                        client_pfd.events = POLLIN;
                        client_pfd.revents = 0;
                        poll_fds.push_back(client_pfd);
                    }
                } else {
                    // Gérer un message d'un client existant
                    handleClient(poll_fds[i].fd);
                }
            }
        }
    }
}

void Server::handleClient(int client_socket) {
    char buffer[1024];
    int bytes_read = read(client_socket, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0) {
        // Client déconnecté
        std::cout << "Client déconnecté." << std::endl;
        close(client_socket);
        // Retirer le descripteur du client de la liste des descripteurs surveillés
        for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it) {
            if (it->fd == client_socket) {
                poll_fds.erase(it);
                break;
            }
        }
    } else {
        buffer[bytes_read] = '\0'; // Terminer la chaîne
        std::cout << "Message reçu : " << buffer << std::endl;
        send(client_socket, buffer, bytes_read, 0); // Écho du message
    }
}
