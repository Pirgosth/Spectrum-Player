#include "visuals.h"

Slider::Slider(sf::Vector2f position, sf::Vector2f size, sf::Color outlineColor, sf::Color fillColor, float thickness){
    outlineBar.setFillColor(sf::Color::Transparent);
    outlineBar.setOutlineColor(outlineColor);
    outlineBar.setOutlineThickness(thickness);
    fillBar.setFillColor(fillColor);
    SetValue(minValue);
    SetPosition(position);
    SetSize(size);
}

void Slider::SetPosition(sf::Vector2f position){
    outlineBar.setPosition(position);
    fillBar.setPosition(position - sf::Vector2f(outlineBar.getOutlineThickness(), outlineBar.getOutlineThickness()));
}

sf::Vector2f Slider::GetPosition(){
    return outlineBar.getPosition();
}

void Slider::SetSize(sf::Vector2f size){
    outlineBar.setSize(size);
    sf::Vector2f fillSize(fillBar.getSize().x, size.y + 2*outlineBar.getOutlineThickness());
    fillBar.setSize(fillSize);
}

sf::Vector2f Slider::GetSize(){
    return outlineBar.getSize();
}

void Slider::SetValue(float value){
    currentValue = std::max(minValue, value);
    currentValue = std::min(maxValue, value);
    sf::Vector2f fillSize = fillBar.getSize();
    fillSize.x = (currentValue - minValue)/(maxValue-minValue)*(outlineBar.getSize().x+2*outlineBar.getOutlineThickness());
    fillBar.setSize(fillSize);
}

void Slider::SetScale(float minValue, float maxValue){
    this->minValue = minValue;
    this->maxValue = maxValue;
}

void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(outlineBar, states);
    target.draw(fillBar, states);
}

RawSpectrum::RawSpectrum(sf::Vector2i position, sf::Vector2i size, sf::SoundBuffer &buffer, sf::Sound &sound, sf::RenderWindow &window){
    this->position = position;
    this->size = size;
    this->window = &window;
    this->buffer = &buffer;
    this->sound = &sound;
    vertexs.setPrimitiveType(sf::LinesStrip);
    vertexs.resize(VISUALS_MAX_SIZE);
    for(int i(0) ; i < VISUALS_MAX_SIZE ; i++) hammingWindow.push_back(0.7-0.23*cos(2*PI*i/(float)VISUALS_MAX_SIZE)) ;
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