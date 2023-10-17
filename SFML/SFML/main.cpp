#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <vector>

const int WINDOW_SIZE = 300;
const int GRID_SIZE = 3;
const int CELL_SIZE = WINDOW_SIZE / GRID_SIZE;
const int PORT = 53000;

enum class Player { None, X, O };

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Tic-Tac-Toe - Client");
    sf::TcpSocket socket;
    if (socket.connect("127.0.0.1", PORT) != sf::Socket::Done) {
        std::cerr << "Gagal terhubung ke server." << std::endl;
        return 1;
    }

    std::vector<std::vector<Player>> board(GRID_SIZE, std::vector<Player>(GRID_SIZE, Player::None));
    Player currentPlayer = Player::X;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (currentPlayer != Player::None) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    int row = mousePos.y / CELL_SIZE;
                    int col = mousePos.x / CELL_SIZE;

                    if (row >= 0 && row < GRID_SIZE && col >= 0 && col < GRID_SIZE && board[row][col] == Player::None) {
                        board[row][col] = currentPlayer;
                        currentPlayer = Player::None;

                        // Kirim langkah klien ke server
                        sf::Packet packet;
                        packet << row << col;
                        socket.send(packet);
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        // Gambar garis-garis papan
        for (int i = 1; i < GRID_SIZE; ++i) {
            sf::Vertex verticalLine[] = {
                sf::Vertex(sf::Vector2f(i * CELL_SIZE, 0)),
                sf::Vertex(sf::Vector2f(i * CELL_SIZE, WINDOW_SIZE))
            };
            sf::Vertex horizontalLine[] = {
                sf::Vertex(sf::Vector2f(0, i * CELL_SIZE)),
                sf::Vertex(sf::Vector2f(WINDOW_SIZE, i * CELL_SIZE))
            };
            window.draw(verticalLine, 2, sf::Lines);
            window.draw(horizontalLine, 2, sf::Lines);
        }

        // Gambar simbol X dan O
        for (int row = 0; row < GRID_SIZE; ++row) {
            for (int col = 0; col < GRID_SIZE; ++col) {
                if (board[row][col] == Player::X) {
                    sf::VertexArray xSymbol(sf::Lines, 4);
                    xSymbol[0].position = sf::Vector2f(col * CELL_SIZE, row * CELL_SIZE);
                    xSymbol[1].position = sf::Vector2f((col + 1) * CELL_SIZE, (row + 1) * CELL_SIZE);
                    xSymbol[2].position = sf::Vector2f(col * CELL_SIZE, (row + 1) * CELL_SIZE);
                    xSymbol[3].position = sf::Vector2f((col + 1) * CELL_SIZE, row * CELL_SIZE);
                    xSymbol[0].color = xSymbol[1].color = xSymbol[2].color = xSymbol[3].color = sf::Color::Red;
                    window.draw(xSymbol);
                }
                else if (board[row][col] == Player::O) {
                    sf::CircleShape oSymbol(CELL_SIZE / 2 - 10);
                    oSymbol.setOutlineThickness(10);
                    oSymbol.setOutlineColor(sf::Color::Green);
                    oSymbol.setFillColor(sf::Color::Transparent);
                    oSymbol.setPosition(col * CELL_SIZE + 10, row * CELL_SIZE + 10);
                    window.draw(oSymbol);
                }
            }
        }

        window.display();
    }

    return 0;
}
