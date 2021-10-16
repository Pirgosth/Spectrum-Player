#ifndef MP3AUDIOSTREAM_H_INCLUDED
#define MP3AUDIOSTREAM_H_INCLUDED

#include <iostream>
#include <string>
#include <SFML/Audio.hpp>
#include <mpg123.h>
#include <algorithm>

#define BUFFER_MULTIPLIER 8

struct BufferDescriptor{
    sf::Int16* buffer = nullptr;
    size_t bufferSize = 0;
};
typedef struct BufferDescriptor BufferDescriptor;

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
    size_t maximumBufferSize = 0;
    BufferDescriptor bufferDescriptors[BUFFER_MULTIPLIER];
    BufferDescriptor sharedBufferDescriptot;
    mpg123_handle *mh = nullptr;
    void reloadFirstBuffers();
    void shiftBuffers();
    void updateSharedBuffer();
public:
    mp3AudioStream(std::string filePath);
    virtual ~mp3AudioStream();
    void clearMemory();
    sf::Time getDuration();
    BufferDescriptor getSharedBufferDescriptor();
};


#endif //MP3AUDIOSTREAM_H_INCLUDED