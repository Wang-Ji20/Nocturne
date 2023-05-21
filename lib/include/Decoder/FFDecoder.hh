#pragma once

extern "C" {

#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/frame.h"
#include "libavutil/mem.h"
}

#include "Decoder/abstractDecoder.hh"

class FFDecoder : public AbstractDecoder {
public:
  FFDecoder(std::string_view filename);
  int getData(char *buffer, int size) override;
  [[nodiscard]] bool getDataInterleave(char **buffer, int *size,
                                       unsigned long *frame) override;
  ~FFDecoder();

private:
  [[nodiscard]] bool nextStream();
  [[nodiscard]] bool precedePacket();
  [[nodiscard]] bool precedeFrame();

  // audio stream format information
  AVFormatContext *formatContext{nullptr};
  int audioStreamIndex = -1;

  // decoder
  AVCodec *codec{nullptr};
  AVCodecContext *codecContext{nullptr};
  AVCodecParameters *codecParameters{nullptr};

  // data
  AVFrame *frame{nullptr};
  AVPacket *packet{nullptr};

  // parser
  // AVCodecParserContext *parser{nullptr};
};