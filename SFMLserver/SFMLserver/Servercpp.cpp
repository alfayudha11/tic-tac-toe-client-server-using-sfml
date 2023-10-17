#include <SFML/Network.hpp>
#include <iostream>
#include <vector>
#include <memory>

const int PORT = 53000;
const int GRID_SIZE = 3;
const int CELL_SIZE = 100;

enum class Player { None, X, O };

int main() {
    sf::TcpListener listener;
    listener.listen(PORT);

    std::vector<std::unique_ptr<sf::TcpSocket>> clients;
    std::vector<Player> playerMarkers = { Player::X, Player::O };
    std::vector<std::vector<Player>> board(GRID_SIZE, std::vector<Player>(GRID_SIZE, Player::None));
    Player currentPlayer = Player::X;
    int currentPlayerIndex = 0;

    std::cout << "Server berjalan menunggu pemain..." << std::endl;

    while (clients.size() < 2) {
        std::unique_ptr<sf::TcpSocket> client(new sf::TcpSocket);
        if (listener.accept(*client) == sf::Socket::Done) {
            clients.push_back(std::move(client));
            std::cout << "Player " << clients.size() << " connected." << std::endl;
        }
    }

    std::cout << "Game started!" << std::endl;

    while (true) {
        // Handle player moves
        for (int i = 0; i < clients.size(); i++) {
            sf::Packet packet;
            if (clients[i]->receive(packet) == sf::Socket::Done) {
                int row, col;
                packet >> row >> col;

                if (board[row][col] == Player::None && currentPlayer == playerMarkers[i]) {
                    board[row][col] = currentPlayer;
                    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                }
            }
        }

        // Send game state to clients
        for (int i = 0; i < clients.size(); i++) {
            sf::Packet gameStatePacket;
            // Mengirimkan seluruh papan permainan
            for (int row = 0; row < GRID_SIZE; ++row) {
                for (int col = 0; col < GRID_SIZE; ++col) {
                    gameStatePacket << static_cast<int>(board[row][col]);
                }
            }
            clients[i]->send(gameStatePacket);
        }
    }

    return 0;
}
