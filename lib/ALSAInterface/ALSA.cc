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

ALSA::ALSA(Decoder &decoder) : decoder(decoder) {
  /* Open PCM device for playback. */
  checksnd(rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0),
           "snd_pcm_open");

  /* Allocate a hardware parameters object. */
  checksnd(snd_pcm_hw_params_malloc(&params), "snd_pcm_hw_params_malloc");

  /* Fill it in with default values. */
  checksnd(snd_pcm_hw_params_any(handle, params), "snd_pcm_hw_params_any");

  /* Set the desired hardware parameters. */

  const WAVHeader &wavHeader = decoder.getHeader();

  /* Interleaved mode */

  checksnd(snd_pcm_hw_params_set_access(handle, params,
                                        SND_PCM_ACCESS_RW_INTERLEAVED),
           "snd_pcm_hw_params_set_access");

  /* set bits_per_sample */

  switch (wavHeader.bits_per_sample) {
  case 8:
    checksnd(snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_U8),
             "snd_pcm_hw_params_set_format");
    break;
  case 16:
    checksnd(
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE),
        "snd_pcm_hw_params_set_format");
    break;

  case 24:
    checksnd(
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S24_LE),
        "snd_pcm_hw_params_set_format");
    break;

  case 32:

    checksnd(
        snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S32_LE),
        "snd_pcm_hw_params_set_format");
    break;

  default:
    throw std::runtime_error("Unsupported bits_per_sample");
  }

  /* set sample rate */
  unsigned int sample_rate = wavHeader.sample_rate;

  checksnd(snd_pcm_hw_params_set_rate_near(handle, params, &sample_rate, &dir),
           "snd_pcm_hw_params_set_rate_near");

  /* set channels */
  checksnd(snd_pcm_hw_params_set_channels(handle, params, wavHeader.channels),
           "snd_pcm_hw_params_set_channels");

  /* Set period size to 32 frames. */
  frames = 32;
  checksnd(
      snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir),
      "snd_pcm_hw_params_set_period_size_near");

  /* Write the parameters to the driver */
  checksnd(snd_pcm_hw_params(handle, params), "snd_pcm_hw_params");

  /* Use a buffer large enough to hold one period */
  checksnd(snd_pcm_hw_params_get_period_size(params, &frames, &dir),
           "snd_pcm_hw_params_get_period_size");

  size = frames * wavHeader.channels * wavHeader.bits_per_sample / 8;

  buffer = new char[size];
}

ALSA::~ALSA() {
  snd_pcm_drain(handle);
  snd_pcm_close(handle);
  delete[] buffer;
}

void ALSA::play() {
  while (decoder.getData(buffer, size)) {
    if (snd_pcm_writei(handle, buffer, frames) == -EPIPE) {
      fprintf(stderr, "underrun occurred");
      snd_pcm_prepare(handle);
    } else if (rc < 0) {
      fprintf(stderr, "error from writei: %s", snd_strerror(rc));
      throw std::runtime_error("error from writei");
    }
  }
}