#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

#include "utils.h"
#include "mp3Handler.h"
#include "visuals.h"

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

void pollEvents(sf::Event &event, sf::Window &window, sf::Sound &sound);
void onKeyPressed(sf::Event &event, sf::Sound &sound);

int main(int argc, char* argv[]){
    std::string programPath = argv[0];
    std::string workingDir = programPath.substr(0, programPath.find_last_of(PATH_SEP)).c_str();

    cd(workingDir.c_str());

    Mp3Handler::InitMp3();

    Mp3Handler mp3Handler;

    sf::Font font;
    if(!font.loadFromFile("Resources/square_rough.ttf")){
        std::cerr << "Could not load font: \"Resources/square_rough.ttf\"" << std::endl;
        return -1;
    }

    std::string path;

    if(argc >= 2){
#ifdef _WIN32
        //FreeConsole();
#endif //_WIN32
        path = argv[1];
        std::cout << path << std::endl;
    }
    else{
        std::cout << "Enter file path: ";
        std::cin >> path;
    }

    sf::SoundBuffer buffer;
    if(tolower_str(getFileExtension(path)) == "mp3"){
        if(!mp3Handler.LoadFromFile(path, buffer)){
            std::cerr << "Could not load file: " << path << std::endl;
            return -1;
        }
    }
    else{
        if(!buffer.loadFromFile(path)){
            std::cerr << "Could not load file: " << path << std::endl;
            return -1;
        }
    }
    sf::Sound sound;
    sound.setBuffer(buffer);

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Spectrum Player");
    sf::Event event;

    Slider playerSlider(sf::Vector2f((1280-800)/2, (720-18)/2+220), sf::Vector2f(800, 18), sf::Color(255, 255, 255, 180), sf::Color(45, 245, 15), -3);
    playerSlider.SetScale(0, buffer.getDuration().asMilliseconds());
    // RawSpectrum rawSpectrum(sf::Vector2i((1280-1000)/2, 360-30), sf::Vector2i(1000, 0), sound, window);
    FftSpectrum fftSpectrum(sf::Vector2f((int)((1280-1000)/2), (int)((720)/2 + 50)), sf::Vector2f(1000, 400), sound);

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

        playerSlider.SetValue(sound.getPlayingOffset().asMilliseconds());
        songPlayingOffsetText.setString(getFormatedTime(sound.getPlayingOffset().asSeconds()));
        // rawSpectrum.Update();
        fftSpectrum.Update();

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