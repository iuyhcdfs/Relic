#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <time.h>
#include <stdlib.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#ifdef _WIN32
    #include <Windows.h>
    #include <gl\gl.h> 
    #include <gl\glu.h>
#endif

#include "Game.h"
#include "RenderUtils.h"

// oh yeah we're goin global
#include "ArtificialMouse.h"
sf::Vector2<double> mousePos = sf::Vector2<double>(0,0);

// Window stuff
double FPS = 60;
double MICROSECONDS_PER_FRAME = 1000000/FPS;
double WIDTH = 1280;
double HEIGHT = 720;

int currentFrame = 0;


Game * currentGame;

void render (Game * game) {
    glLoadIdentity();
    //glUseProgram(0);

    game->render(RenderUtils::DisplayState(WIDTH, HEIGHT, currentFrame, mousePos));
}

void renderingThread(sf::Window * window) {

    window->setActive(true);

    glMatrixMode(GL_MODELVIEW);

    sf::Clock clock;

    while (window->isOpen()) {
        clock.restart();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render(currentGame);

        window->display();

        if (currentFrame % 60 == 0) {
            std::cout << 1000000 / clock.restart().asMicroseconds() << " FPS" << std::endl;
        }
    }
}

int main (int argc, char ** argv) {

    std::cout << "Relic C++ Version" << std::endl;

    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "Relic", sf::Style::Default, sf::ContextSettings(32));
    window.setVerticalSyncEnabled(false);
    window.setMouseCursorVisible(false);
    window.setKeyRepeatEnabled(false);

    RenderUtils::init(WIDTH, HEIGHT);

    Game game = Game();
    currentGame = &game;

    window.setActive(false);
    sf::Thread thread(&renderingThread, &window);
    thread.launch();

    sf::Clock clock;

    srand(time(NULL));

    //sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

    while (true) {
        clock.restart();
        sf::Event event;
        // TODO pass events down to game
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                break;
            } else if (event.type == sf::Event::Resized) {
                RenderUtils::resize(event.size.width, event.size.height);
                WIDTH = event.size.width;
                HEIGHT = event.size.height;
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) { // ESC - closes program
                    exit(0);
                }
                game.handleSFEvent(event);
            }
            else {
                game.handleSFEvent(event);
            }
        }
/*
    std::cout << mousePos.x << "\n";
    std::cout << mousePos.y << "\n";
*/
        game.update(currentFrame, mousePos);
 
        currentFrame += 1;

        mousePos.x += (sf::Mouse::getPosition(window).x / WIDTH  - 0.5);
        mousePos.y -= (sf::Mouse::getPosition(window).y / HEIGHT - 0.5);
        sf::Mouse::setPosition(sf::Vector2i(WIDTH / 2, HEIGHT / 2), window);

        double d = MICROSECONDS_PER_FRAME - clock.restart().asMicroseconds();

        sf::sleep(sf::microseconds(d < 0 ? 0 : d));
    }

    return 0;
}