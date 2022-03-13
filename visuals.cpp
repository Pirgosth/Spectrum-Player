#include "visuals.h"
#include "mp3Handler.h"

Slider::Slider(sf::Vector2f position, sf::Vector2f size, sf::Color outlineColor, sf::Color fillColor, float thickness)
{
    outlineBar.setFillColor(sf::Color::Transparent);
    outlineBar.setOutlineColor(outlineColor);
    outlineBar.setOutlineThickness(thickness);
    fillBar.setFillColor(fillColor);
    SetValue(minValue);
    SetPosition(position);
    SetSize(size);
}

void Slider::SetPosition(sf::Vector2f position)
{
    outlineBar.setPosition(position);
    fillBar.setPosition(position - sf::Vector2f(outlineBar.getOutlineThickness(), outlineBar.getOutlineThickness()));
}

sf::Vector2f Slider::GetPosition()
{
    return outlineBar.getPosition();
}

void Slider::SetSize(sf::Vector2f size)
{
    outlineBar.setSize(size);
    sf::Vector2f fillSize(fillBar.getSize().x, size.y + 2 * outlineBar.getOutlineThickness());
    fillBar.setSize(fillSize);
}

sf::Vector2f Slider::GetSize()
{
    return outlineBar.getSize();
}

void Slider::SetValue(float value)
{
    currentValue = std::max(minValue, value);
    currentValue = std::min(maxValue, value);
    sf::Vector2f fillSize = fillBar.getSize();
    fillSize.x = (currentValue - minValue) / (maxValue - minValue) * (outlineBar.getSize().x + 2 * outlineBar.getOutlineThickness());
    fillBar.setSize(fillSize);
}

void Slider::SetScale(float minValue, float maxValue)
{
    this->minValue = minValue;
    this->maxValue = maxValue;
}

void Slider::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(outlineBar, states);
    target.draw(fillBar, states);
}

RawSpectrum::RawSpectrum(sf::Vector2i position, sf::Vector2i size, sf::Sound &sound, sf::RenderWindow &window)
{
    this->position = position;
    this->size = size;
    this->window = &window;
    this->sound = &sound;
    vertexs.setPrimitiveType(sf::LinesStrip);
    vertexs.resize(BUFFER_SIZE);
    for (int i(0); i < BUFFER_SIZE; i++)
        hammingWindow.push_back(0.77 - 0.23 * cos(2 * PI * i / (float)BUFFER_SIZE));
}

void RawSpectrum::Update()
{
    int mark = sound->getPlayingOffset().asSeconds() * sound->getBuffer()->getSampleRate() * sound->getBuffer()->getChannelCount();
    if (mark + BUFFER_SIZE < sound->getBuffer()->getSampleCount())
    {
        for (int i(mark); i < mark + BUFFER_SIZE; i++)
        {
            vertexs[i - mark] = sf::Vertex(sf::Vector2f(position) + sf::Vector2f((i - mark) / (float)BUFFER_SIZE * size.x, hammingWindow[i - mark] * sound->getBuffer()->getSamples()[i] * 0.005), sf::Color(0, 255, 0, 50));
        }
    }
}

void RawSpectrum::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(vertexs, states);
}

FftSpectrum::FftSpectrum(sf::Vector2f position, sf::Vector2f size, Mp3Handler &handler) : d(exp(log(BUFFER_SIZE / 6) / size.x))
{
    // std::cout << "Dilatation coefficient: " << d << std::endl;
    this->position = position;
    this->size = size;

    this->handler = &handler;

    vertexs.setPrimitiveType(sf::Lines);

    sampleRate = handler.getSampleRate();
}

void FftSpectrum::Update()
{
    BufferDescriptor currentBufferDescriptor = handler->getSharedBufferDescriptor();

    sf::Int16 *buffer = currentBufferDescriptor.buffer;
    sampleCount = currentBufferDescriptor.bufferSize;
    bufferSize = BUFFER_SIZE < sampleCount ? BUFFER_SIZE : sampleCount;
    if (sampleCount <= 0 || buffer == nullptr)
    {
        // std::cout << "No samples in current buffer" << std::endl;
        return;
    }
    samples.resize(bufferSize);
    vertexs.resize(bufferSize);
    std::vector<float> hammingWindow;
    for (int i(0); i < bufferSize; i++)
        hammingWindow.push_back(0.54 - 0.46 * cos(2 * PI * i / (float)bufferSize));

    for (int i(0); i < bufferSize; i++)
    {
        samples[i] = Complex(buffer[i] * hammingWindow[i], 0);
    }


    bin = CArray(samples.data(), bufferSize);
    fft(bin);

    vertexs.clear();

    for (float i(3); i < bufferSize / 2.f; i *= d)
    {
        sf::Vector2f samplePosition(log(i / 3) / log(bufferSize / 6.f) * size.x, std::min(size.y / 2, (float)std::abs(bin[(int)i]) / MAX_BARS * size.y));
        samplePosition.x += position.x;
        vertexs.append(sf::Vertex(sf::Vector2f((int)(samplePosition.x), (int)(position.y - samplePosition.y)), sf::Color(22, 166, 96)));
        vertexs.append(sf::Vertex(sf::Vector2f((int)(samplePosition.x), (int)position.y), sf::Color(22, 166, 96)));
        vertexs.append(sf::Vertex(sf::Vector2f((int)(samplePosition.x), (int)position.y), sf::Color(22, 166, 96, 100)));
        vertexs.append(sf::Vertex(sf::Vector2f((int)(samplePosition.x), (int)(position.y + samplePosition.y / 2)), sf::Color(22, 166, 96, 0)));
    }
}

void FftSpectrum::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
    target.draw(vertexs, states);
}