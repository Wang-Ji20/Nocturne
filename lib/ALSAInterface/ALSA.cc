// Nocturne 2023
// identification: lib/ALSAInterface/ALSA.cc
//

#include "ALSAInterface/ALSA.hh"
#include <stdexcept>

static void checksnd(int rc, const char *msg) {
  if (rc < 0) {
    fprintf(stderr, "%s: %s)", msg, snd_strerror(rc));
    throw std::runtime_error(msg);
  }
}

[[maybe_unused]] static void checkUNIX(int rc, const char *msg) {
  if (rc < 0) {
    fprintf(stderr, "%s: %s)", msg, strerror(errno));
    throw std::runtime_error(msg);
  }
}

ALSA::ALSA(AbstractDecoder &decoder, bool naive, snd_pcm_uframes_t frames)
    : frames{frames}, decoder{decoder}, alsaHeader{decoder.getHeader()} {

  /* Open PCM device for playback. */
  checksnd(snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0),
           "snd_pcm_open");

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  checksnd(snd_pcm_hw_params_any(handle, params), "snd_pcm_hw_params_any");

  /* Set the desired hardware parameters. */

  /* access mode */

  checksnd(
      snd_pcm_hw_params_set_access(handle, params, alsaHeader.accessMethod),
      "snd_pcm_hw_params_set_access");

  /* set bits_per_sample */

  checksnd(snd_pcm_hw_params_set_format(handle, params, alsaHeader.format),
           "snd_pcm_hw_params_set_format");

  /* set sample rate */
  unsigned int sample_rate = alsaHeader.sample_rate;

  checksnd(snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir),
           "snd_pcm_hw_params_set_rate_near");

  /* set channels */
  checksnd(snd_pcm_hw_params_set_channels(handle, params, alsaHeader.channels),
           "snd_pcm_hw_params_set_channels");

  /* Write the parameters to the driver */
  checksnd(snd_pcm_hw_params(handle, params), "snd_pcm_hw_params");

  size = frames * alsaHeader.channels * alsaHeader.bits_per_sample / 8;

  // music player control unit
  snd_mixer_open(&mixer, 0);
  snd_mixer_attach(mixer, "default");
  snd_mixer_selem_register(mixer, NULL, NULL);
  snd_mixer_load(mixer);

  fprintf(stderr, "playing this %d Hz %d channels %d bits_per_sample audio\n",
          sample_rate, alsaHeader.channels, alsaHeader.bits_per_sample);
  fprintf(stderr, "period size is %lu\n", frames);

  if (!naive)
    playThread = std::make_unique<std::thread>(&ALSA::playLoop, this);
}

ALSA::~ALSA() {
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  snd_mixer_close(mixer);
}

void ALSA::playLoop() {
  auto playFunc = alsaHeader.accessMethod == SND_PCM_ACCESS_RW_INTERLEAVED
                      ? &ALSA::playInterleave
                      : &ALSA::playPlanar;
  do {
    std::unique_lock<std::mutex> lock(mutex);
    if (control == PAUSE)
      cv.wait(lock, [this] { return control == PLAY; });
    else
      lock.unlock();
  } while (((this->*playFunc)()));
}

bool ALSA::playInterleave() {
  char* buffer;
  if (decoder.getDataInterleave(&buffer, &size, &frames)) {
    int retv = 0;
    if ((retv = snd_pcm_writei(handle, buffer, frames)) == -EPIPE) {
      fprintf(stderr, "underrun occurred\n");
      int code = snd_pcm_prepare(handle);
      if (code < 0) {
        fprintf(stderr, "prepare failed, code is %d\n", code);
      } else {
        fprintf(stderr, "prepared\n");
      }
    } else if (retv < 0) {
      fprintf(stderr, "error from writei: %s\n", snd_strerror(retv));
      throw std::runtime_error("error from writei");
    } else if (retv != (int)frames) {
      fprintf(stderr, "short write, write %d frames\n", retv);
    }
  } else {
    return false;
  }
  return true;
}

bool ALSA::playPlanar() {
  char **buffers;
  if (decoder.getDataPlanar(&buffers, &size, &frames)) {
    int retv = 0;
    if ((retv = snd_pcm_writen(handle, (void**)buffers, frames)) == -EPIPE) {
      fprintf(stderr, "underrun occurred\n");
      int code = snd_pcm_prepare(handle);
      if (code < 0) {
        fprintf(stderr, "prepare failed, code is %d\n", code);
      } else {
        fprintf(stderr, "prepared\n");
      }
    } else if (retv < 0) {
      fprintf(stderr, "error from writei: %s\n", snd_strerror(retv));
      throw std::runtime_error("error from writei");
    } else if (retv != (int)frames) {
      fprintf(stderr, "short write, write %d frames\n", retv);
    }
  } else {
    return false;
  }
  return true;
}

void ALSA::naivePlay() {
  int retv = 0;
  char* buffer = nullptr;
  while (decoder.getData(buffer, size)) {
    if ((retv = snd_pcm_writei(handle, buffer, frames)) == -EPIPE) {
      // fprintf(stderr, "underrun occurred\n");
      snd_pcm_prepare(handle);
    } else if (retv < 0) {
      fprintf(stderr, "error from writei: %s\n", snd_strerror(retv));
      throw std::runtime_error("error from writei");
    }
  }
}

void ALSA::play() {
  {
    std::scoped_lock<std::mutex> lock(mutex);
    snd_pcm_pause(handle, 0);
    control = PLAY;
  }
  cv.notify_one();
}

void ALSA::pause() {
  {
    std::scoped_lock<std::mutex> lock(mutex);
    snd_pcm_pause(handle, 1);
    control = PAUSE;
  }
  cv.notify_one();
}

void ALSA::setVolume(int volume) {
  snd_mixer_elem_t *elem = snd_mixer_first_elem(mixer);
  while (elem != NULL) {
    if (snd_mixer_elem_get_type(elem) == SND_MIXER_ELEM_SIMPLE &&
        snd_mixer_selem_is_active(elem)) {
      const char *name = snd_mixer_selem_get_name(elem);
      if (strcmp(name, "Master") == 0) {
        break;
      }
    }
    elem = snd_mixer_elem_next(elem);
  }
  long max = 512, vol;
  vol = (long)(max * volume / 100.0f); // volume is a float between 0 and 1
  snd_mixer_selem_set_playback_volume_all(elem, vol);
}