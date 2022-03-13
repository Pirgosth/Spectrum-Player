#ifndef MP3HANDLER_H_INCLUDED
#define MP3HANDLER_H_INCLUDED

#include <SFML/Audio.hpp>
#include <SFML/Config.hpp>
#include <iostream>
#include <mpg123.h>
#include <out123.h>
#include <string>

#define BUFFER_MULTIPLIER 8

struct BufferDescriptor {
  sf::Int16 *buffer = nullptr;
  size_t bufferSize = 0;
};
typedef struct BufferDescriptor BufferDescriptor;

struct MP3Data {
  int channelCount = 0;
  long sampleRate = 1;
  off_t sampleCount = 0;
  int encoding = 0;
  int framesize = 1;
};

class Mp3Handler {
private:
  mpg123_handle *mh = nullptr;
  out123_handle *ao = nullptr;
  std::string path;
  MP3Data data;
  size_t buffer_size = 0;
  BufferDescriptor bufferDescriptors[BUFFER_MULTIPLIER];
  BufferDescriptor sharedBufferDescriptor;
  int playStatus = 0;
  bool isPlaying = false;
  void reloadFirstBuffers();
  void shiftBuffers();
  void updateSharedBuffer();

public:
  static bool initMp3();
  Mp3Handler();
  bool update();
  bool openFromFile(std::string path);
  sf::Time getDuration();
  sf::Time getPlayingOffset();
  bool getStatus();
  void play();
  void pause();
  void stop();
  void setPlayingOffset(sf::Time offset);
  int getSampleRate();
  BufferDescriptor getSharedBufferDescriptor();
  ~Mp3Handler();
};

#endif // MP3HANDLER_H_INCLUDED