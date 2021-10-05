#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

#include "utils.h"
#include "mp3Handler.h"
#include "visuals.h"
#include "mp3AudioStream.hpp"

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

void pollEvents(sf::Event &event, sf::Window &window, mp3AudioStream &stream);
void onKeyPressed(sf::Event &event, mp3AudioStream &stream);

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

    mp3AudioStream mp3Music(path);

    std::cout << "Total duration of track: " << mp3Music.getDuration().asSeconds() << " seconds" << std::endl;

    // sf::SoundBuffer buffer;
    // if(tolower_str(getFileExtension(path)) == "mp3"){
    //     if(!mp3Handler.LoadFromFile(path, buffer)){
    //         std::cerr << "Could not load file: " << path << std::endl;
    //         return -1;
    //     }
    // }
    // else{
    //     if(!buffer.loadFromFile(path)){
    //         std::cerr << "Could not load file: " << path << std::endl;
    //         return -1;
    //     }
    // }
    // sf::Sound sound;
    // sound.setBuffer(buffer);

    sf::RenderWindow window(sf::VideoMode(1280, 720), "Spectrum Player");
    sf::Event event;

    Slider playerSlider(sf::Vector2f((1280-800)/2, (720-18)/2+220), sf::Vector2f(800, 18), sf::Color(255, 255, 255, 180), sf::Color(45, 245, 15), -3);
    playerSlider.SetScale(0, mp3Music.getDuration().asMilliseconds());
    // RawSpectrum rawSpectrum(sf::Vector2i((1280-1000)/2, 360-30), sf::Vector2i(1000, 0), sound, window);
    FftSpectrum fftSpectrum(sf::Vector2f((int)((1280-1000)/2), (int)((720)/2 + 50)), sf::Vector2f(1000, 400), mp3Music);

    sf::Text songNameText(getFileName(path), font);
    songNameText.setPosition((window.getSize().x - songNameText.getGlobalBounds().width)/2 , 65);

    sf::Text songDurationText(getFormatedTime(mp3Music.getDuration().asSeconds()), font);
    songDurationText.setPosition(playerSlider.GetPosition().x + playerSlider.GetSize().x + 15, playerSlider.GetPosition().y + (playerSlider.GetSize().y-songDurationText.getGlobalBounds().height)/2 -5);
    sf::Text songPlayingOffsetText(getFormatedTime(0), font);
    songPlayingOffsetText.setPosition(playerSlider.GetPosition().x - songPlayingOffsetText.getGlobalBounds().width - 15, playerSlider.GetPosition().y + (playerSlider.GetSize().y-songPlayingOffsetText.getGlobalBounds().height)/2 -5);

    mp3Music.play();

    while(window.isOpen()){
        while(window.pollEvent(event)){
            pollEvents(event, window, mp3Music);
        }

        playerSlider.SetValue(mp3Music.getPlayingOffset().asMilliseconds());
        songPlayingOffsetText.setString(getFormatedTime(mp3Music.getPlayingOffset().asSeconds()));
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

void pollEvents(sf::Event &event, sf::Window &window, mp3AudioStream &stream){
    switch(event.type){
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            onKeyPressed(event, stream);
            break;
        default:
            break;
    }
}

void onKeyPressed(sf::Event &event, mp3AudioStream &stream){
    switch(event.key.code){
        case sf::Keyboard::P:
            switch(stream.getStatus()){
                case sf::SoundSource::Playing:
                    stream.pause();
                    break;
                default:
                    stream.play();
                    break;
            }
            break;
        case sf::Keyboard::S:
            stream.stop();
            break;
        case sf::Keyboard::Left:
            stream.setPlayingOffset(sf::seconds(std::max(stream.getPlayingOffset().asSeconds() - 5, 0.0f)));
            break;
        case sf::Keyboard::Right:
            stream.setPlayingOffset(sf::seconds(std::min(stream.getPlayingOffset().asSeconds() + 5, stream.getDuration().asSeconds())));
            break;
        default:
            break;
    }
}