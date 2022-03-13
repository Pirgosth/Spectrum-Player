#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

// #include "mp3AudioStream.hpp"
#include "mp3Handler.h"
#include "utils.h"
#include "visuals.h"

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>
#define cwd _getcwd
#define cd _chdir
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#endif

void pollEvents(sf::Event &event, sf::Window &window, Mp3Handler &handler);
void onKeyPressed(sf::Event &event, Mp3Handler &handler);

int main(int argc, char *argv[]) {
  std::string programPath = argv[0];
  std::string workingDir =
      programPath.substr(0, programPath.find_last_of(PATH_SEP)).c_str();

  cd(workingDir.c_str());

  Mp3Handler::initMp3();

  Mp3Handler mp3Handler;

  sf::Font font;
  if (!font.loadFromFile("Resources/square_rough.ttf")) {
    std::cerr << "Could not load font: \"Resources/square_rough.ttf\""
              << std::endl;
    return -1;
  }

  std::string path;

  if (argc >= 2) {
    path = argv[1];
    std::cout << path << std::endl;
  } else {
    std::cout << "Enter file path: ";
    std::cin >> path;
  }

  if (!mp3Handler.openFromFile(path))
    return -1;

  std::cout << "Total duration of track: "
            << mp3Handler.getDuration().asSeconds() << " seconds" << std::endl;

  sf::RenderWindow window(sf::VideoMode(1280, 720), "Spectrum Player");
  sf::Event event;

  Slider playerSlider(sf::Vector2f((1280 - 800) / 2, (720 - 18) / 2 + 220),
                      sf::Vector2f(800, 18), sf::Color(255, 255, 255, 180),
                      sf::Color(22, 166, 96), -3);
  playerSlider.SetScale(0, mp3Handler.getDuration().asMilliseconds());
  // RawSpectrum rawSpectrum(sf::Vector2i((1280-1000)/2, 360-30),
  // sf::Vector2i(1000, 0), sound, window);
    FftSpectrum fftSpectrum(
        sf::Vector2f((int)((1280 - 1000) / 2), (int)((720) / 2 + 50)),
        sf::Vector2f(1000, 400), mp3Handler);

  sf::Text songNameText(getFileName(path), font);
  songNameText.setPosition(
      (window.getSize().x - songNameText.getGlobalBounds().width) / 2, 65);

  sf::Text songDurationText(
      getFormatedTime(mp3Handler.getDuration().asSeconds()), font);
  songDurationText.setPosition(playerSlider.GetPosition().x +
                                   playerSlider.GetSize().x + 15,
                               playerSlider.GetPosition().y +
                                   (playerSlider.GetSize().y -
                                    songDurationText.getGlobalBounds().height) /
                                       2 -
                                   5);
  sf::Text songPlayingOffsetText(getFormatedTime(0), font);
  songPlayingOffsetText.setPosition(
      playerSlider.GetPosition().x -
          songPlayingOffsetText.getGlobalBounds().width - 15,
      playerSlider.GetPosition().y +
          (playerSlider.GetSize().y -
           songPlayingOffsetText.getGlobalBounds().height) /
              2 -
          5);

  std::cout << sizeof(sf::Int16) << std::endl;
  std::cout << sizeof(unsigned char) << std::endl;

  while (window.isOpen()) {
    while (window.pollEvent(event)) {
      pollEvents(event, window, mp3Handler);
    }

    playerSlider.SetValue(mp3Handler.getPlayingOffset().asMilliseconds());
    songPlayingOffsetText.setString(
        getFormatedTime(mp3Handler.getPlayingOffset().asSeconds()));
    // rawSpectrum.Update();
    mp3Handler.update();
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

void pollEvents(sf::Event &event, sf::Window &window, Mp3Handler &handler) {
  switch (event.type) {
  case sf::Event::Closed:
    window.close();
    break;
  case sf::Event::KeyPressed:
    onKeyPressed(event, handler);
    break;
  default:
    break;
  }
}

void onKeyPressed(sf::Event &event, Mp3Handler &handler) {
  switch (event.key.code) {
  case sf::Keyboard::P:
    if (handler.getStatus())
      handler.pause();
    else
      handler.play();
    break;
  case sf::Keyboard::S:
    handler.stop();
    break;
  case sf::Keyboard::Left:
    handler.setPlayingOffset(
        sf::seconds(std::max(handler.getPlayingOffset().asSeconds() - 5, 0.0f)));
    break;
  case sf::Keyboard::Right:
    handler.setPlayingOffset(
        sf::seconds(std::min(handler.getPlayingOffset().asSeconds() + 5,
                             handler.getDuration().asSeconds())));
    break;
  default:
    break;
  }
}