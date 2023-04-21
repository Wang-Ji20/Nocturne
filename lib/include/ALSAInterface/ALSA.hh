// Nocturne 2023
// identification: lib/include/ALSAInterface/ALSA.hh
//

#pragma once

#include "WAVDecoder/Decoder.hh"

#include <alsa/asoundlib.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

class ALSA {
public:
  ALSA(Decoder &decoder, bool naive = false, snd_pcm_uframes_t frames = 32);
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
  int rc;
  int size;
  char *buffer;
  Decoder &decoder;

  std::unique_ptr<std::thread> playThread;
  enum { PLAY, PAUSE } control;
  std::mutex mutex;
  std::condition_variable cv;
};