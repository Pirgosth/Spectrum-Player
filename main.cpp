#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

#include "utils.h"
#include "mp3Handler.h"
#include "visuals.h"

#include <unistd.h>

void pollEvents(sf::Event &event, sf::Window &window, sf::Sound &sound);
void onKeyPressed(sf::Event &event, sf::Sound &sound);

int main(int argc, char* argv[]){

    std::string programPath = argv[0];

    chdir(programPath.substr(0, programPath.find_last_of('/')).c_str());

    Mp3Handler::InitMp3();

    Mp3Handler mp3Handler;

    sf::Font font;
    if(!font.loadFromFile("Resources/square_rough.ttf")){
        std::cerr << "Could not load font" << std::endl;
        return -1;
    }

    std::string path;

    if(argc >= 2){
        path = argv[1];
    }
    else{
        std::cout << "Enter file path: ";
        std::cin >> path;
    }

    sf::SoundBuffer buffer;

    if(!mp3Handler.LoadFromFile(path, buffer)){
        std::cerr << "Could not load file: " << path << std::endl;
        return -1;
    }

    sf::Sound sound;
    sound.setBuffer(buffer);

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Spectrum Player");
    sf::Event event;

    Slider playerSlider(sf::Vector2f((1280-800)/2, (720-22)/2+220), sf::Vector2f(800, 22), sf::Color(255, 255, 255, 180), sf::Color(255, 140, 0, 200), -3);
    playerSlider.SetScale(0, buffer.getDuration().asMilliseconds());
    // RawSpectrum rawSpectrum(sf::Vector2i((1280-1000)/2, 360-30), sf::Vector2i(1000, 0), sound, window);
    FftSpectrum fftSpectrum(sf::Vector2f((1280-900)/2, (720)/2 + 50), sf::Vector2f(800, 400), sound, window);

    sf::Text songNameText(getFileName(path), font);
    songNameText.setPosition((window.getSize().x - songNameText.getGlobalBounds().width)/2 , 65);

    sf::Text songDurationText(getFormatedTime(buffer.getDuration().asSeconds()), font);
    songDurationText.setPosition(playerSlider.GetPosition().x + playerSlider.GetSize().x + 15, playerSlider.GetPosition().y + (playerSlider.GetSize().y-songDurationText.getGlobalBounds().height)/2 -5);
    sf::Text songPlayingOffsetText(getFormatedTime(0), font);
    songPlayingOffsetText.setPosition(playerSlider.GetPosition().x - songPlayingOffsetText.getGlobalBounds().width - 15, playerSlider.GetPosition().y + (playerSlider.GetSize().y-songPlayingOffsetText.getGlobalBounds().height)/2 -5);

    

    sound.play();

    while(window.isOpen()){
        while(window.pollEvent(event)){
            pollEvents(event, window, sound);
        }

        if(true){//if(sound.getStatus() == sf::SoundSource::Playing){ //TODO Check if elements refresh when song ends.
            playerSlider.SetValue(sound.getPlayingOffset().asMilliseconds());
            songPlayingOffsetText.setString(getFormatedTime(sound.getPlayingOffset().asSeconds()));
            // rawSpectrum.Update();
            fftSpectrum.Update();
        }

        window.clear();

        window.draw(playerSlider);
        // window.draw(rawSpectrum);
        window.draw(fftSpectrum);

        window.draw(songDurationText);
        window.draw(songPlayingOffsetText);
        window.draw(songNameText);

        window.display();
    }

    return 0;
}

void pollEvents(sf::Event &event, sf::Window &window, sf::Sound &sound){
    switch(event.type){
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            onKeyPressed(event, sound);
            break;
        default:
            break;
    }
}

void onKeyPressed(sf::Event &event, sf::Sound &sound){
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
        case sf::Keyboard::S:
            sound.stop();
            break;
        case sf::Keyboard::Left:
            sound.setPlayingOffset(sf::seconds(std::max(sound.getPlayingOffset().asSeconds() - 5, 0.0f)));
            break;
        case sf::Keyboard::Right:
            sound.setPlayingOffset(sf::seconds(std::min(sound.getPlayingOffset().asSeconds() + 5, sound.getBuffer()->getDuration().asSeconds())));
            break;
        default:
            break;
    }
}