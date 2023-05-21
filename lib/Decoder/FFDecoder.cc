#include "Decoder/FFDecoder.hh"

// NOTE: After all, cpp and c are two different languages
extern "C" {
#include "alsa/asoundlib.h"

static snd_pcm_access_t getAccessMethod(AVCodecContext *ctx) {
  AVSampleFormat format = ctx->sample_fmt;
  if (format == AV_SAMPLE_FMT_FLTP || format == AV_SAMPLE_FMT_DBLP ||
      format == AV_SAMPLE_FMT_S32P || format == AV_SAMPLE_FMT_S16P ||
      format == AV_SAMPLE_FMT_U8P) {
    return SND_PCM_ACCESS_RW_NONINTERLEAVED;
  }
  return SND_PCM_ACCESS_RW_INTERLEAVED;
}

static snd_pcm_format_t getFormat(AVSampleFormat format) {
  switch (format) {
  case AV_SAMPLE_FMT_U8:
    return SND_PCM_FORMAT_U8;
  case AV_SAMPLE_FMT_S16:
    return SND_PCM_FORMAT_S16;
  case AV_SAMPLE_FMT_S32:
    return SND_PCM_FORMAT_S32;
  case AV_SAMPLE_FMT_FLTP:
  case AV_SAMPLE_FMT_FLT:
    return SND_PCM_FORMAT_FLOAT;
  case AV_SAMPLE_FMT_DBLP:
  case AV_SAMPLE_FMT_DBL:
    return SND_PCM_FORMAT_FLOAT64;
  default:
    return SND_PCM_FORMAT_UNKNOWN;
  }
}

// open input file, and allocate format context
FFDecoder::FFDecoder(std::string_view filename) {
  formatContext = avformat_alloc_context();
  if (!formatContext) {
    throw std::runtime_error("Cannot allocate format context");
  }

  if (avformat_open_input(&formatContext, filename.data(), nullptr, nullptr) <
      0) {
    throw std::runtime_error("Cannot open input");
  }

  if (avformat_find_stream_info(formatContext, nullptr) < 0) {
    throw std::runtime_error("Cannot find stream info");
  }

  frame = av_frame_alloc();
  if (!frame) {
    throw std::runtime_error("Cannot allocate frame");
  }

  packet = av_packet_alloc();
  if (!packet) {
    throw std::runtime_error("Cannot allocate packet");
  }

  if (!nextStream()) {
    throw std::runtime_error("Cannot find audio stream");
  }
}

// seek to next audio stream
// returns true if there is a next stream, false otherwise
bool FFDecoder::nextStream() {
  for (audioStreamIndex = audioStreamIndex + 1;
       audioStreamIndex < int(formatContext->nb_streams); audioStreamIndex++) {
    if (formatContext->streams[audioStreamIndex]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      break;
    }
  }

  if (audioStreamIndex == -1 ||
      audioStreamIndex >= int(formatContext->nb_streams)) {
    return false;
  }

  if (codecParameters != nullptr) {
    avcodec_parameters_free(&codecParameters);
  }
  codecParameters = formatContext->streams[audioStreamIndex]->codecpar;

  codec = avcodec_find_decoder(
      formatContext->streams[audioStreamIndex]->codecpar->codec_id);
  if (!codec) {
    throw std::runtime_error("Cannot find decoder");
  }

  if (codecContext != nullptr) {
    avcodec_free_context(&codecContext);
  }

  codecContext = avcodec_alloc_context3(codec);
  if (!codecContext) {
    throw std::runtime_error("Cannot allocate codec context");
  }

  if (avcodec_parameters_to_context(codecContext, codecParameters) < 0) {
    throw std::runtime_error("Cannot copy codec params to codec context");
  }

  // fill ALSA
  alsaHeader.bits_per_sample =
      8 * av_get_bytes_per_sample(codecContext->sample_fmt);
  alsaHeader.channels = codecContext->channels;
  alsaHeader.sample_rate = codecContext->sample_rate;
  alsaHeader.format = getFormat(codecContext->sample_fmt);
  alsaHeader.accessMethod = getAccessMethod(codecContext);

  if (avcodec_open2(codecContext, codec, nullptr) < 0) {
    throw std::runtime_error("Cannot open codec");
  }

  return true;
}

// false if end of file
// true if there is more data
// throws if error
bool FFDecoder::precedePacket() {
  do {
    switch (av_read_frame(formatContext, packet)) {
    case 0:
      break;
    case AVERROR_EOF:
      if (!nextStream()) {
        return false;
      }
      continue;
    default:
      throw std::runtime_error("Cannot read frame");
    }
    // invalid frame, continue
  } while (packet->stream_index != audioStreamIndex);

  // send packet to decoder
  int retv = 0;
  if ((retv = avcodec_send_packet(codecContext, packet)) < 0) {
    char av_strerrorbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(retv, av_strerrorbuf, AV_ERROR_MAX_STRING_SIZE);
    fprintf(stderr, "avcodec_send_packet: %s\n", av_strerrorbuf);
    throw std::runtime_error("Cannot send packet to decoder");
  }

  return true;
}

// return true if there is more data
// return false if decoder has no more data
// throws if error
bool FFDecoder::precedeFrame() {
start:
  int ret = 0;
  ret = avcodec_receive_frame(codecContext, frame);
  if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
    if (!precedePacket()) {
      return false;
    }
    goto start;
  } else if (ret < 0) {
    throw std::runtime_error("Cannot receive frame from decoder");
  }
  return true;
}

// assert size > frame size
// we can use std::span if cpp20
int FFDecoder::getData(char *buffer, int size) {
  if (precedeFrame() && frame->nb_samples * frame->channels < size) {
    memcpy(buffer, frame->data[0], frame->nb_samples * frame->channels);
  }
  return frame->nb_samples * frame->channels;
}

bool FFDecoder::getDataInterleave(char **buffer, int *size,
                                  unsigned long *frames) {
  if (precedeFrame()) {
    *buffer = (char *)frame->data[0];
    *frames = frame->nb_samples;
    *size = frame->nb_samples * frame->channels *
            av_get_bytes_per_sample(codecContext->sample_fmt);
  } else {
    return false;
  }
  return true;
}

bool FFDecoder::getDataPlanar(char ***buffers, int *size,
                              unsigned long *frames) {
  if (precedeFrame()) {
    *buffers = (char **)frame->data;
    *frames = frame->nb_samples;
    *size = av_get_bytes_per_sample(codecContext->sample_fmt);
  } else {
    return false;
  }
  return true;
}

FFDecoder::~FFDecoder() {
  avformat_close_input(&formatContext);
  av_frame_free(&frame);
  av_packet_free(&packet);
  avcodec_free_context(&codecContext);
}

} // extern "C"