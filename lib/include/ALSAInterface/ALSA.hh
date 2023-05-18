// Nocturne 2023
// identification: lib/include/ALSAInterface/ALSA.hh
//

#pragma once

#include "Decoder/abstractDecoder.hh"

#include <alsa/asoundlib.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

class ALSA {
public:
  // TODO: this magic number 5512 sucks, but I cannot fix it.
  ALSA(AbstractDecoder &decoder, bool naive = false,
       snd_pcm_uframes_t frames = 5512);
  ~ALSA();

  ALSA(const ALSA &) = delete;
  ALSA(ALSA &&) = delete;
  ALSA &operator=(const ALSA &) = delete;
  ALSA &operator=(ALSA &&) = delete;

  void naivePlay();
  void play();
  void pause();
  void setVolume(int volume);

private:
  void playLoop();

  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_mixer_t *mixer;
  snd_pcm_uframes_t frames;
  int dir;
  int size;
  char *buffer;
  AbstractDecoder &decoder;

  std::unique_ptr<std::thread> playThread;
  enum { PLAY, PAUSE } control;
  std::mutex mutex;
  std::condition_variable cv;
};