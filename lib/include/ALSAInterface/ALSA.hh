// Nocturne 2023
// identification: lib/include/ALSAInterface/ALSA.hh
//

#pragma once

#include "Decoder/abstractDecoder.hh"
#include "Effector/Debussy.hh"

#include <alsa/asoundlib.h>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

class ALSA {
public:
  ALSA(Debussy &debussy);
  ~ALSA();

  ALSA(const ALSA &) = delete;
  ALSA(ALSA &&) = delete;
  ALSA &operator=(const ALSA &) = delete;
  ALSA &operator=(ALSA &&) = delete;

  // play the music in a separate thread
  void Lachaise();
  // play music in current thread
  void CemeteryOfInnocents();

  void resume();
  void pause();
  void setVolume(int volume);

private:
  bool playInterleave();

  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_mixer_t *mixer;

  snd_pcm_uframes_t frames{5512};
  int dir;
  int size;

  Debussy &debussy;
  const ALSAHeader &alsaHeader;

  std::unique_ptr<std::thread> patheon;
  enum { PLAY, PAUSE } control;
  std::mutex mutex;
  std::condition_variable cv;
};