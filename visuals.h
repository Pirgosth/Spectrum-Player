#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <vector>
#include <math.h>

#define VISUALS_MAX_SIZE 16834

const double PI = 3.141592653589793238460 ;

class Slider: public sf::Drawable{
private:
    float currentValue = 0;
    float minValue = 0;
    float maxValue = 1;
    sf::RectangleShape outlineBar;
    sf::RectangleShape fillBar;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    Slider(sf::Vector2f position, sf::Vector2f size, sf::Color outlineColor, sf::Color fillColor, float thickness);
    void SetPosition(sf::Vector2f position);
    sf::Vector2f GetPosition();
    void SetSize(sf::Vector2f size);
    sf::Vector2f GetSize();
    void SetValue(float value);
    void SetScale(float minValue, float maxValue);
};

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