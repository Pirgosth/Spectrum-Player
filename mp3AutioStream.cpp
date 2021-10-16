#include "mp3AudioStream.hpp"

void mp3AudioStream::reloadFirstBuffers()
{
    for (int i(1); i < BUFFER_MULTIPLIER; i++)
    {
        size_t decodedOffsetSize = 0;
        mpg123_read(mh, bufferDescriptors[i].buffer, maximumBufferSize, &decodedOffsetSize);
        bufferDescriptors[i].bufferSize = decodedOffsetSize / sizeof(sf::Int16);
    }
}

void mp3AudioStream::shiftBuffers()
{
    sf::Int16 *oldFirstBuffer = bufferDescriptors[0].buffer;
    size_t oldFirstBufferSize = bufferDescriptors[0].bufferSize;

    for (int i(0); i < BUFFER_MULTIPLIER - 1; i++)
    {
        bufferDescriptors[i].buffer = bufferDescriptors[i + 1].buffer;
        bufferDescriptors[i].bufferSize = bufferDescriptors[i + 1].bufferSize;
    }

    bufferDescriptors[BUFFER_MULTIPLIER - 1].buffer = oldFirstBuffer;
    bufferDescriptors[BUFFER_MULTIPLIER - 1].bufferSize = oldFirstBufferSize;
}

mp3AudioStream::mp3AudioStream(std::string filePath) : sf::SoundStream()
{
    mh = mpg123_new(nullptr, &err);
    if (mh == nullptr)
    {
        std::cerr << "Handle initialisation goes wrong: " << mpg123_plain_strerror(err) << std::endl;
    }

    if (mpg123_open(mh, filePath.c_str()) != MPG123_OK || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
    {
        std::cerr << "Could not open or read file " << filePath << " : " << mpg123_plain_strerror(err) << std::endl;
        clearMemory();
        return;
    }
    initialize(channels, rate);
    mpg123_scan(mh);
    maximumBufferSize = mpg123_outblock(mh);
    totalSampleCount = mpg123_length(mh);
    std::cout << "rate: " << rate << ", channels: " << channels << ", encoding: " << encoding << std::endl;
    std::cout << "Maximum size of one decoded frame buffer: " << maximumBufferSize << std::endl;
    std::cout << "Total number of sample for this track: " << totalSampleCount << std::endl;

    for (int i(0); i < BUFFER_MULTIPLIER; i++)
    {
        bufferDescriptors[i].buffer = new sf::Int16[maximumBufferSize];
    }

    sharedBufferDescriptot.buffer = new sf::Int16[maximumBufferSize * BUFFER_MULTIPLIER];

    reloadFirstBuffers();
}

mp3AudioStream::~mp3AudioStream()
{
    clearMemory();
}

void mp3AudioStream::updateSharedBuffer()
{
    size_t currentBufferOffset = 0;

    sharedBufferDescriptot.bufferSize = 0;

    for (int i(0); i < BUFFER_MULTIPLIER; i++)
        sharedBufferDescriptot.bufferSize += bufferDescriptors[i].bufferSize;

    for (int i(0); i < BUFFER_MULTIPLIER; i++)
    {
        currentBufferOffset += i > 0 ? bufferDescriptors[i - 1].bufferSize : 0;
        std::copy(bufferDescriptors[i].buffer, bufferDescriptors[i].buffer + bufferDescriptors[i].bufferSize, sharedBufferDescriptot.buffer + currentBufferOffset);
    }
}

bool mp3AudioStream::onGetData(Chunk &data)
{
    shiftBuffers();

    updateSharedBuffer();

    data.samples = bufferDescriptors[0].buffer;
    data.sampleCount = bufferDescriptors[0].bufferSize;

    if (bufferDescriptors[0].bufferSize * sizeof(sf::Int16) != maximumBufferSize)
    {
        // std::cout << "End of track" << std::endl;
        // mpg123_seek_frame(mh, 0, SEEK_SET);
        return false;
    }

    size_t decodedOffsetSize = 0;
    mpg123_read(mh, bufferDescriptors[BUFFER_MULTIPLIER - 1].buffer, maximumBufferSize, &decodedOffsetSize);
    bufferDescriptors[BUFFER_MULTIPLIER - 1].bufferSize = decodedOffsetSize / sizeof(sf::Int16);

    return true;
}

void mp3AudioStream::onSeek(sf::Time timeOffset)
{
    off_t seekOffset = rate * timeOffset.asSeconds();
    mpg123_seek(mh, static_cast<off_t>(timeOffset.asSeconds() * rate), SEEK_SET);
    reloadFirstBuffers();
}

void mp3AudioStream::clearMemory()
{
    for (int i(0); i < BUFFER_MULTIPLIER; i++)
    {
        delete[] bufferDescriptors[i].buffer;
    }

    delete[] sharedBufferDescriptot.buffer;

    mpg123_close(mh);
    mpg123_delete(mh);
    mh = nullptr;
}

sf::Time mp3AudioStream::getDuration()
{
    return sf::seconds((float)totalSampleCount / rate);
}

BufferDescriptor mp3AudioStream::getSharedBufferDescriptor()
{
    return sharedBufferDescriptot;
}