#include "mp3Handler.h"
#include <SFML/Config.hpp>
#include <SFML/System/Time.hpp>
#include <mpg123.h>
#include <out123.h>

bool Mp3Handler::initMp3() {
  int err = mpg123_init();
  if (err != MPG123_OK) {
    std::cerr << mpg123_plain_strerror(err) << std::endl;
    return false;
  }
  return true;
}

Mp3Handler::Mp3Handler() {
  int err = MPG123_OK;
  mh = mpg123_new(NULL, &err);
  if (!mh) {
    std::cerr << "Unable to create mpg123 handle: "
              << mpg123_plain_strerror(err) << std::endl;
  }
  ao = out123_new();
  if (!ao) {
    std::cerr << "Cannot create output handle" << std::endl;
  }
}

Mp3Handler::~Mp3Handler() {
  mpg123_close(mh);
  mpg123_delete(mh);
  out123_close(ao);
  out123_del(ao);
  for (int i(0); i < BUFFER_MULTIPLIER; i++) {
    delete[] bufferDescriptors[i].buffer;
  }

  delete[] sharedBufferDescriptor.buffer;

  mh = nullptr;
  ao = nullptr;
}

void Mp3Handler::reloadFirstBuffers() {
  for (int i(1); i < BUFFER_MULTIPLIER; i++) {
    bufferDescriptors[i].bufferSize = 0;
    mpg123_read(mh, bufferDescriptors[i].buffer, buffer_size,
                &bufferDescriptors[i].bufferSize);
                
    bufferDescriptors[i].bufferSize /= sizeof(sf::Int16);
  }
}

void Mp3Handler::shiftBuffers() {
  sf::Int16 *oldFirstBuffer = bufferDescriptors[0].buffer;
  size_t oldFirstBufferSize = bufferDescriptors[0].bufferSize;

  for (int i(0); i < BUFFER_MULTIPLIER - 1; i++) {
    bufferDescriptors[i].buffer = bufferDescriptors[i + 1].buffer;
    bufferDescriptors[i].bufferSize = bufferDescriptors[i + 1].bufferSize;
  }

  bufferDescriptors[BUFFER_MULTIPLIER - 1].buffer = oldFirstBuffer;
  bufferDescriptors[BUFFER_MULTIPLIER - 1].bufferSize = oldFirstBufferSize;
}

void Mp3Handler::updateSharedBuffer() {
  size_t currentBufferOffset = 0;

  sharedBufferDescriptor.bufferSize = 0;

  for (int i(0); i < BUFFER_MULTIPLIER; i++)
    sharedBufferDescriptor.bufferSize += bufferDescriptors[i].bufferSize;

  for (int i(0); i < BUFFER_MULTIPLIER; i++) {
    currentBufferOffset += i > 0 ? bufferDescriptors[i - 1].bufferSize : 0;
    std::copy(bufferDescriptors[i].buffer,
              bufferDescriptors[i].buffer + bufferDescriptors[i].bufferSize,
              sharedBufferDescriptor.buffer + currentBufferOffset);
  }
}

bool Mp3Handler::openFromFile(std::string path) {
  this->path = path;

  if (mh) {
    mpg123_close(mh);
  }

  if (mpg123_open(mh, path.c_str()) != MPG123_OK) {
    std::cerr << mpg123_strerror(mh) << std::endl;
    return false;
  }

  mpg123_getformat(mh, &data.sampleRate, &data.channelCount, &data.encoding);

  data.sampleCount = mpg123_length(mh);

  if (out123_open(ao, nullptr, nullptr) != OUT123_OK) {
    std::cerr << "Trouble with out123: " << out123_strerror(ao) << std::endl;
    return false;
  }

  if (out123_start(ao, data.sampleRate, data.channelCount, data.encoding) ||
      out123_getformat(ao, NULL, NULL, NULL, &data.framesize)) {
    std::cerr << "Cannot start output / get framesize: " << out123_strerror(ao)
              << std::endl;
    return false;
  }

  buffer_size = mpg123_outblock(mh);

  for (int i(0); i < BUFFER_MULTIPLIER; i++) {
    bufferDescriptors[i].buffer = new sf::Int16[buffer_size];
  }

  sharedBufferDescriptor.buffer =
      new sf::Int16[buffer_size * BUFFER_MULTIPLIER];

  reloadFirstBuffers();

  return true;
}

bool Mp3Handler::update() {

  if (!isPlaying)
    return false;

  shiftBuffers();
  updateSharedBuffer();

  if (!bufferDescriptors[0].bufferSize || playStatus != MPG123_OK)
    return false;
  size_t played = out123_play(ao, bufferDescriptors[0].buffer,
                            bufferDescriptors[0].bufferSize * sizeof(sf::Int16));
  if (played != bufferDescriptors[0].bufferSize * sizeof(sf::Int16)) {
    fprintf(stderr,
            "Warning: written less than gotten from libmpg123: %li != %li\n",
            (long)played, (long)bufferDescriptors[0].bufferSize);
  }

  if (bufferDescriptors[0].bufferSize * sizeof(sf::Int16) != buffer_size) {
    // std::cout << "End of track" << std::endl;
    // mpg123_seek_frame(mh, 0, SEEK_SET);
    stop();
    return true;
  }

  mpg123_read(mh, bufferDescriptors[BUFFER_MULTIPLIER - 1].buffer, buffer_size,
              &bufferDescriptors[BUFFER_MULTIPLIER - 1].bufferSize);

  bufferDescriptors[BUFFER_MULTIPLIER - 1].bufferSize /= sizeof(sf::Int16);

  return true;
}

sf::Time Mp3Handler::getDuration() {
  return sf::seconds((float)data.sampleCount / data.sampleRate);
}

sf::Time Mp3Handler::getPlayingOffset() {
  return sf::seconds((float)mpg123_tell(mh) / data.sampleRate);
}

bool Mp3Handler::getStatus() { return isPlaying; }

void Mp3Handler::play() { isPlaying = true; }

void Mp3Handler::pause() { isPlaying = false; }

void Mp3Handler::stop() {
  pause();
  setPlayingOffset(sf::seconds(0));
}

int Mp3Handler::getSampleRate() { return data.sampleRate; }

BufferDescriptor Mp3Handler::getSharedBufferDescriptor() {
  return sharedBufferDescriptor;
}

void Mp3Handler::setPlayingOffset(sf::Time offset) {
  off_t seekOffset = data.sampleRate * offset.asSeconds();
  mpg123_seek(mh, static_cast<off_t>(offset.asSeconds() * data.sampleRate),
              SEEK_SET);
  reloadFirstBuffers();
}