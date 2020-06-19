#include "mp3Handler.h"

bool Mp3Handler::InitMp3(){
    int err = mpg123_init();
    if(err != MPG123_OK){
        std::cerr << mpg123_plain_strerror(err) << std::endl;
        return false;
    }
    return true;
}

Mp3Handler::Mp3Handler(){
    int err = MPG123_OK;
    handle = mpg123_new(NULL, &err);
    if (!handle)
    {
        std::cerr << "Unable to create mpg123 handle: " << mpg123_plain_strerror(err) << std::endl;
    }
}

Mp3Handler::~Mp3Handler(){
    mpg123_close(handle);
    mpg123_delete(handle);
    handle = nullptr;
}

bool Mp3Handler::LoadFromFile(std::string path, sf::SoundBuffer &soundBuffer){
    this->path = path;

    if(handle){
        mpg123_close(handle);
    }

    if(mpg123_open(handle, path.c_str()) != MPG123_OK){
        std::cerr << mpg123_strerror(handle) << std::endl;
        return false;
    }
    QueryData();
    DecodeBuffer();
    bool result = LoadSoundBuffer(soundBuffer);
    ClearBuffers();
    return result;
}

void Mp3Handler::QueryData(){
    int channelCount = 0;
    long sampleRate = 0;

    mpg123_getformat(handle, &sampleRate, &channelCount, nullptr);

    data = {
        .channelCount = channelCount,
        .sampleRate = sampleRate,
    };
}

void Mp3Handler::DecodeBuffer(){
    rawBuffer = new unsigned char[MAX_BUFFER_SIZE];
    decodedBuffer = new unsigned char[MAX_BUFFER_SIZE];

    size_t sampleCount;
    
    mpg123_decode(handle, rawBuffer, MAX_BUFFER_SIZE, decodedBuffer, MAX_BUFFER_SIZE, &sampleCount);
    data.sampleCount = sampleCount;
}

bool Mp3Handler::LoadSoundBuffer(sf::SoundBuffer &soundBuffer){
    unsigned char* finalBuffer = new unsigned char[data.sampleCount];
    for(int i(0); i<data.sampleCount; i++){
        finalBuffer[i] = decodedBuffer[i];
    }
    if(!soundBuffer.loadFromSamples((sf::Int16*)finalBuffer, data.sampleCount/sizeof(sf::Int16), data.channelCount, data.sampleRate)){
        std::cerr << "Unable to load buffer" << std::endl;
        return false;
    }
    return true;
}

void Mp3Handler::ClearBuffers(){
    if(rawBuffer){
        delete [] rawBuffer;
        rawBuffer = nullptr;
    }

    if(decodedBuffer){
        delete [] decodedBuffer;
        decodedBuffer = nullptr;
    }
}

