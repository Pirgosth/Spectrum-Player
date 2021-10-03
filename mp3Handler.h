#ifndef MP3HANDLER_H_INCLUDED
#define MP3HANDLER_H_INCLUDED

#include <iostream>
#include <string>
#include <mpg123.h>
#include <SFML/Audio.hpp>

#define MAX_BUFFER_SIZE 300000000 //300Mo

struct MP3_DATA{
    int channelCount = 0;
    long sampleRate = 0;
    size_t sampleCount;
};

class Mp3Handler
{
private:
    mpg123_handle *handle = nullptr;
    std::string path;
    MP3_DATA data;
    unsigned char* rawBuffer;
    unsigned char* decodedBuffer;
    void QueryData();
    void DecodeBuffer();
    bool LoadSoundBuffer(sf::SoundBuffer &soundBuffer);
    void ClearBuffers();
public:
    static bool InitMp3();
    Mp3Handler();
    bool LoadFromFile(std::string path, sf::SoundBuffer &soundBuffer);
    ~Mp3Handler();
};

#endif //MP3HANDLER_H_INCLUDED