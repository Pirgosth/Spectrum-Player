#include "mp3AudioStream.hpp"

mp3AudioStream::mp3AudioStream(std::string filePath): sf::SoundStream()
{
    mh = mpg123_new(nullptr, &err);
    if(mh == nullptr)
    {
        std::cerr << "Handle initialisation goes wrong: " << mpg123_plain_strerror(err) << std::endl;
    }

    if(mpg123_open(mh, filePath.c_str()) != MPG123_OK || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
    {
        std::cerr << "Could not open or read file " << filePath <<  " : " << mpg123_plain_strerror(err) << std::endl;
        clearMemory();
        return;
    }
    initialize(channels, rate);
    mpg123_scan(mh);
    decodedBufferSize = mpg123_outblock(mh);
    totalSampleCount = mpg123_length(mh);
    std::cout << "rate: " << rate << ", channels: " << channels << ", encoding: " << encoding << std::endl;
    std::cout << "Maximum size of one decoded frame buffer: " << decodedBufferSize << std::endl;
    std::cout << "Total number of sample for this track: " << totalSampleCount << std::endl;
}

mp3AudioStream::~mp3AudioStream()
{
    clearMemory();
}

bool mp3AudioStream::onGetData(Chunk& data)
{
    char* decodedBuffer[decodedBufferSize];
    size_t decodedOffsetSize = 0;
    mpg123_read(mh, &decodedBuffer, decodedBufferSize, &decodedOffsetSize);
    data.samples = (sf::Int16*)decodedBuffer;
    data.sampleCount = decodedOffsetSize/sizeof(sf::Int16);
    bool remainingSamples = (decodedOffsetSize == decodedBufferSize);
    if(!remainingSamples)
    {
        // std::cout << "End of track" << std::endl;
        // mpg123_seek_frame(mh, 0, SEEK_SET);
    }
    return remainingSamples;
}

void mp3AudioStream::onSeek(sf::Time timeOffset)
{
    off_t seekOffset = rate*timeOffset.asSeconds();
    mpg123_seek_frame(mh, mpg123_timeframe(mh, timeOffset.asSeconds()), SEEK_SET);
}

void mp3AudioStream::clearMemory()
{
    mpg123_close(mh);
    mpg123_delete(mh);
    mh = nullptr;
}

sf::Time mp3AudioStream::getDuration()
{
    return sf::seconds((float)totalSampleCount/rate);
}