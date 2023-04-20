// Nocturne 2023
// identification: lib/include/ALSAInterface/ALSA.hh
//

#pragma once

#include "WAVDecoder/Decoder.hh"

#include <alsa/asoundlib.h>

class ALSA {
public:
  ALSA(Decoder &decoder);
  ~ALSA();

  ALSA(const ALSA &) = delete;
  ALSA(ALSA &&) = delete;
  ALSA &operator=(const ALSA &) = delete;
  ALSA &operator=(ALSA &&) = delete;

  void play();

private:
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  snd_pcm_uframes_t frames;
  int dir;
  int rc;
  int size;
  char *buffer;
  Decoder &decoder;
};