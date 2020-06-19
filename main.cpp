#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

#include "mp3Handler.h"

int main(){

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Spectrum");
    sf::Event event;

    std::string path = "Resources/rampage.mp3";

    Mp3Handler::InitMp3();

    Mp3Handler mp3Handler;

    sf::SoundBuffer buffer;

    if(!mp3Handler.LoadFromFile(path, buffer)){
        std::cerr << "Could not load file: " << path << std::endl;
        return -1;
    }

    std::cout << "Buffer duration: " << buffer.getDuration().asSeconds() << std::endl;

    sf::Sound sound;
    sound.setBuffer(buffer);
    sound.play();

    while(window.isOpen()){
        while(window.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                default:
                    break;
            }
        }

        window.clear();
        window.display();
    }

    return 0;
}