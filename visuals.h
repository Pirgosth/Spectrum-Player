#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <vector>
#include <math.h>

#define VISUALS_MAX_SIZE 16834

const double PI = 3.141592653589793238460 ;

class RawSpectrum{
private:
    sf::Vector2i size;
    sf::Vector2i position;
    sf::RenderWindow *window;
    sf::SoundBuffer *buffer;
    sf::Sound *sound;
    sf::VertexArray vertexs;
    std::vector<float> hammingWindow;
public:
    RawSpectrum(sf::Vector2i position, sf::Vector2i size, sf::SoundBuffer &buffer, sf::Sound &sound, sf::RenderWindow &window);
    void Update();
    void Draw();
};

#endif //VISUALS_H_INCLUDED