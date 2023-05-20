#include "Decoder/FFDecoder.hh"

// NOTE: After all, cpp and c are two different languages
extern "C" {

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
  for (size_t i = audioStreamIndex + 1; i < formatContext->nb_streams; i++) {
    if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
      audioStreamIndex = i;
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
  alsaHeader.bits_per_sample = codecContext->bits_per_raw_sample;
  alsaHeader.channels = codecContext->channels;
  alsaHeader.sample_rate = codecContext->sample_rate;

  if (avcodec_open2(codecContext, codec, nullptr) < 0) {
    throw std::runtime_error("Cannot open codec");
  }

  if (parser != nullptr) {
    av_parser_close(parser);
  }
  parser = av_parser_init(
      formatContext->streams[audioStreamIndex]->codecpar->codec_id);
  if (!parser) {
    throw std::runtime_error("Cannot init parser");
  }

  return true;
}

// false if end of file
// true if there is more data
// throws if error
bool FFDecoder::precedePacket() {
start:
  do {
    switch (av_read_frame(formatContext, packet)) {
    case 0:
      break;
    case AVERROR_EOF:
      if (!nextStream()) {
        return false;
      }
      goto start;
    default:
      throw std::runtime_error("Cannot read frame");
    }
    // invalid frame, continue
  } while (packet->stream_index != audioStreamIndex);

  // send packet to parser
  //   if (av_parser_parse2(parser, codecContext, &packet->data, &packet->size,
  //                        packet->data, packet->size, AV_NOPTS_VALUE,
  //                        AV_NOPTS_VALUE, 0) < 0) {
  //     throw std::runtime_error("Cannot parse packet");
  //   }


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
int FFDecoder::getData(char *buffer, int size) {
  if (precedeFrame() && frame->nb_samples * frame->channels < size) {
    memcpy(buffer, frame->data[0], frame->nb_samples*frame->channels);
  }
  return frame->nb_samples * frame->channels;
}

FFDecoder::~FFDecoder() {
  avformat_close_input(&formatContext);
  av_frame_free(&frame);
  av_packet_free(&packet);
  av_parser_close(parser);
  avcodec_free_context(&codecContext);
}

} // extern "C"