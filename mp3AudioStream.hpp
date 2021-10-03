#ifndef MP3AUDIOSTREAM_H_INCLUDED
#define MP3AUDIOSTREAM_H_INCLUDED

#include <iostream>
#include <string>
#include <SFML/Audio.hpp>
#include <mpg123.h>

class mp3AudioStream: public sf::SoundStream
{
private:
    virtual bool onGetData(Chunk& data);
    virtual void onSeek(sf::Time timeOffset);

    int err = MPG123_OK;
    long rate = 0;
    int channels = 0;
    int encoding = 0;
    long totalSampleCount = 0;
    size_t decodedBufferSize = 0;
    mpg123_handle *mh = nullptr;
public:
    mp3AudioStream(std::string filePath);
    virtual ~mp3AudioStream();
    void clearMemory();
    sf::Time getDuration();
};


#endif //MP3AUDIOSTREAM_H_INCLUDED