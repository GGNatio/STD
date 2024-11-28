#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

class Rond {
public:
    sf::CircleShape shape;
    float speedX, speedY;

    Rond(float radius, float posX, float posY) {
        shape.setRadius(radius);
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(posX, posY);
        
        speedX = (rand() % 5 + 1) * (rand() % 2 == 0 ? 1 : -1);
        speedY = (rand() % 5 + 1) * (rand() % 2 == 0 ? 1 : -1);
    }

    void update() {
        shape.move(speedX, speedY);
    }

    void resetPosition(int windowWidth, int windowHeight) {
        shape.setPosition(rand() % windowWidth, rand() % windowHeight);
    }
};

class Vaisseau {
public:
    sf::RectangleShape shape;
    float speed;

    Vaisseau(float startX, float startY) {
        shape.setSize(sf::Vector2f(30.f, 15.f));
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(startX, startY);
        speed = 5.f;
    }

    void move(sf::Keyboard::Key key) {
        if (key == sf::Keyboard::Up)    shape.move(0, -speed);
        if (key == sf::Keyboard::Down)  shape.move(0, speed);
        if (key == sf::Keyboard::Left)  shape.move(-speed, 0);
        if (key == sf::Keyboard::Right) shape.move(speed, 0);
    }

    sf::FloatRect getBounds() {
        return shape.getGlobalBounds();
    }
};

enum class GameState { Menu, Jeu, GameOver };

int main() {
    
    sf::Font font;
    if (!font.loadFromFile("../assets/Jaro-Regular.ttf")) {
        return -1;
    }

    const int windowWidth = 1920;
    const int windowHeight = 1080;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Jeu Asteroids");
    window.setFramerateLimit(60);
    sf::Texture t;
    t.loadFromFile("../assets/back.png");
    sf::Sprite s(t);
    srand(static_cast<unsigned int>(time(0)));

    GameState gameState = GameState::Menu;

    Vaisseau vaisseau(windowWidth / 2.f - 15, windowHeight / 2.f - 7.5f);
    std::vector<Rond> ronds;

    int score = 0;                // Variable pour stocker le score
    sf::Clock scoreClock;         // Horloge pour mesurer le temps écoulé

    auto resetGame = [&]() {
        ronds.clear();
        for (int i = 0; i < 10; ++i) {
            ronds.push_back(Rond(rand() % 20 + 15, rand() % windowWidth, rand() % windowHeight));
        }
        vaisseau.shape.setPosition(windowWidth / 2.f - 15, windowHeight / 2.f - 7.5f);
        score = 0;
        scoreClock.restart();     // Réinitialise le compteur de temps
        gameState = GameState::Jeu;
        };

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (gameState == GameState::Menu) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                    resetGame();
                }
            }
            else if (gameState == GameState::GameOver) {
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                    resetGame();
                }
            }
        }

        if (gameState == GameState::Jeu) {
            window.draw(s);
            for (auto& rond : ronds) {
                rond.update();

                if (rond.shape.getPosition().x < 0 || rond.shape.getPosition().x > windowWidth ||
                    rond.shape.getPosition().y < 0 || rond.shape.getPosition().y > windowHeight) {
                    rond.resetPosition(windowWidth, windowHeight);
                }

                if (vaisseau.getBounds().intersects(rond.shape.getGlobalBounds())) {
                    gameState = GameState::GameOver;
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    vaisseau.move(sf::Keyboard::Up);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  vaisseau.move(sf::Keyboard::Down);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  vaisseau.move(sf::Keyboard::Left);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) vaisseau.move(sf::Keyboard::Right);

            // Mise à jour du score toutes les secondes
            if (scoreClock.getElapsedTime().asSeconds() >= 1.f) {
                score++;
                scoreClock.restart();
            }
        }

        window.clear();

        if (gameState == GameState::Menu) {
            sf::Text title("Bienvenue dans Asteroids", font, 70);
            title.setFillColor(sf::Color::White);
            title.setPosition(windowWidth / 2.f - title.getGlobalBounds().width / 2.f, 300);

            sf::Text instructions("Appuyez sur ENTREE pour jouer", font, 40);
            instructions.setFillColor(sf::Color::White);
            instructions.setPosition(windowWidth / 2.f - instructions.getGlobalBounds().width / 2.f, 500);

            window.draw(title);
            window.draw(instructions);
        }
        else if (gameState == GameState::Jeu) {
            window.draw(s);
            for (const auto& rond : ronds) {
                window.draw(rond.shape);
            }
            window.draw(vaisseau.shape);

            sf::Text scoreText("Score: " + std::to_string(score), font, 40);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition(10, 10);
            window.draw(scoreText);
        }
        else if (gameState == GameState::GameOver) {
            sf::Text gameOverText("Game Over!", font, 70);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(windowWidth / 2.f - gameOverText.getGlobalBounds().width / 2.f, 300);

            sf::Text replayText("Appuyez sur R pour rejouer", font, 40);
            replayText.setFillColor(sf::Color::White);
            replayText.setPosition(windowWidth / 2.f - replayText.getGlobalBounds().width / 2.f, 500);

            sf::Text finalScoreText("Score: " + std::to_string(score), font, 50);
            finalScoreText.setFillColor(sf::Color::White);
            finalScoreText.setPosition(windowWidth / 2.f - finalScoreText.getGlobalBounds().width / 2.f, 400);

            window.draw(gameOverText);
            window.draw(replayText);
            window.draw(finalScoreText);
        }
        
        window.display();
    }

    return 0;
}
