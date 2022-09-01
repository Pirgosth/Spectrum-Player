#ifndef VISUALS_H_INCLUDED
#define VISUALS_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <iostream>
#include <vector>
#include <math.h>
#include <cstring>

#include "mp3Handler.h"
#include "utils.h"

#define BUFFER_SIZE 16384

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

class RawSpectrum: public sf::Drawable{
private:
    sf::Vector2i size;
    sf::Vector2i position;
    sf::RenderWindow *window;
    sf::Sound *sound;
    sf::VertexArray vertexs;
    std::vector<float> hammingWindow;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    RawSpectrum(sf::Vector2i position, sf::Vector2i size, sf::Sound &sound, sf::RenderWindow &window);
    void Update();
};

class FftSpectrum: public sf::Drawable{
private:
    const int MAX_BARS = 100000000;
    const float d;

    sf::Vector2f size;
    sf::Vector2f position;
    sf::VertexArray vertexs;

    std::vector<float> hammingWindow;

    Mp3Handler* handler;
    
    std::vector<Complex> samples;
    CArray bin;

    int sampleCount;
    int sampleRate;
    int bufferSize;

    // std::vector<float> hammingWindow;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    FftSpectrum(sf::Vector2f position, sf::Vector2f size, Mp3Handler &handler);
    void Update();
};

#endif //VISUALS_H_INCLUDED