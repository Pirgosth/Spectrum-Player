#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

#include "mp3Handler.h"
#include "visuals.h"

int main(){

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Spectrum");
    sf::Event event;

    std::string path = "Resources/leap.mp3";

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

    RawSpectrum rawSpectrum(sf::Vector2i((1280-800)/2, 360), sf::Vector2i(800, 0), buffer, sound, window);

    sound.play();

    while(window.isOpen()){
        while(window.pollEvent(event)){
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed:
                    switch(event.key.code){
                        case sf::Keyboard::P:
                            switch(sound.getStatus()){
                                case sf::SoundSource::Playing:
                                    sound.pause();
                                    break;
                                default:
                                    sound.play();
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        rawSpectrum.Update();

        window.clear();

        rawSpectrum.Draw();

        window.display();
    }

    return 0;
}