#include "visuals.h"

RawSpectrum::RawSpectrum(sf::Vector2i position, sf::Vector2i size, sf::SoundBuffer &buffer, sf::Sound &sound, sf::RenderWindow &window){
    this->position = position;
    this->size = size;
    this->window = &window;
    this->buffer = &buffer;
    this->sound = &sound;
    vertexs.setPrimitiveType(sf::LinesStrip);
    vertexs.resize(VISUALS_MAX_SIZE);
    for(int i(0) ; i < VISUALS_MAX_SIZE ; i++) hammingWindow.push_back(0.54-0.46*cos(2*PI*i/(float)VISUALS_MAX_SIZE)) ;
}

void RawSpectrum::Update(){
    int mark = sound->getPlayingOffset().asSeconds()*buffer->getSampleRate()*buffer->getChannelCount();
    if(mark+VISUALS_MAX_SIZE < buffer->getSampleCount()){
        for(int i(mark); i<mark + VISUALS_MAX_SIZE; i++){
            vertexs[i-mark] = sf::Vertex(sf::Vector2f(position) + sf::Vector2f((i-mark)/(float)VISUALS_MAX_SIZE*size.x, hammingWindow[i-mark]*buffer->getSamples()[i]*0.005), sf::Color(0, 255, 0, 50));
        }
    }
}

void RawSpectrum::Draw(){
    window->draw(vertexs);
}