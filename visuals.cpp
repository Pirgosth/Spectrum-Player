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

RawSpectrum::RawSpectrum(sf::Vector2i position, sf::Vector2i size, sf::Sound &sound, sf::RenderWindow &window){
    this->position = position;
    this->size = size;
    this->window = &window;
    this->sound = &sound;
    vertexs.setPrimitiveType(sf::LinesStrip);
    vertexs.resize(VISUALS_MAX_SIZE);
    for(int i(0) ; i < VISUALS_MAX_SIZE ; i++) hammingWindow.push_back(0.77-0.23*cos(2*PI*i/(float)VISUALS_MAX_SIZE)) ;
}

void RawSpectrum::Update(){
    int mark = sound->getPlayingOffset().asSeconds()*sound->getBuffer()->getSampleRate()*sound->getBuffer()->getChannelCount();
    if(mark+VISUALS_MAX_SIZE < sound->getBuffer()->getSampleCount()){
        for(int i(mark); i<mark + VISUALS_MAX_SIZE; i++){
            vertexs[i-mark] = sf::Vertex(sf::Vector2f(position) + sf::Vector2f((i-mark)/(float)VISUALS_MAX_SIZE*size.x, hammingWindow[i-mark]*sound->getBuffer()->getSamples()[i]*0.005), sf::Color(0, 255, 0, 50));
        }
    }
}

void RawSpectrum::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(vertexs, states);
}

FftSpectrum::FftSpectrum(sf::Vector2f position, sf::Vector2f size, sf::Sound &sound, sf::RenderWindow &window){
    this->position = position;
    this->size = size;
    this->sound = &sound;
    this->window = &window;
    bin.resize(VISUALS_MAX_SIZE);
    vertexs.setPrimitiveType(sf::Lines);
    vertexs.resize(VISUALS_MAX_SIZE);
    for(int i(0) ; i < VISUALS_MAX_SIZE ; i++) hammingWindow.push_back(0.54-0.46*cos(2*PI*i/(float)VISUALS_MAX_SIZE)) ;
}

void FftSpectrum::Update(){
    int mark = sound->getPlayingOffset().asSeconds()*sound->getBuffer()->getSampleRate()*sound->getBuffer()->getChannelCount();
    if(mark+VISUALS_MAX_SIZE < sound->getBuffer()->getSampleCount()){
        for(int i(mark); i<mark + VISUALS_MAX_SIZE; i++){
            bin[i-mark] = Complex((float)sound->getBuffer()->getSamples()[i]*hammingWindow[i-mark], 0);
        }
    }

    fft(bin);

    vertexs.clear();

    for(float i(3) ; i < VISUALS_MAX_SIZE/2.f ; i*=1.01)
	{
		sf::Vector2f samplePosition(log(i/3)/log(VISUALS_MAX_SIZE/6.f), std::min(size.y/2 , std::max(1.0f, 0.4f*((int)log(i)))*(float)std::abs(bin[(int)i])/VISUALS_MAX_SIZE/MAX_BARS*size.y)) ;
        //std::cout << (int)(samplePosition.x*size.x) << std::endl;
        vertexs.append(sf::Vertex(position+sf::Vector2f((int)(samplePosition.x*size.x),(int)(-samplePosition.y)),sf::Color::White)) ;
		vertexs.append(sf::Vertex(position+sf::Vector2f((int)(samplePosition.x * size.x),0),sf::Color::White)) ;
		vertexs.append(sf::Vertex(position+sf::Vector2f((int)(samplePosition.x * size.x),0),sf::Color(255,255,255,100))) ;
		vertexs.append(sf::Vertex(position+sf::Vector2f((int)(samplePosition.x * size.x),(int)(samplePosition.y/2)),sf::Color(255,255,255,0))) ;
	}
}

void FftSpectrum::draw(sf::RenderTarget& target, sf::RenderStates states) const{
    target.draw(vertexs, states);
}