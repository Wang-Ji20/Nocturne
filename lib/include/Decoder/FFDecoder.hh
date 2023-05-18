#pragma once

#include "libavcodec/avcodec.h"
#include "libavutil/frame.h"
#include "libavutil/mem.h"

#include "Decoder/abstractDecoder.hh"

class FFDecoder: public AbstractDecoder {
public:
    FFDecoder(std::string_view filename);
    int getData(char *buffer, int size) override;
    ~FFDecoder() { file.close(); }

private:
    std::fstream file;
    AVCodec *codec;
    AVCodecContext *codecContext;
    AVFrame *frame;
    AVPacket *packet;
    AVFrame *swrFrame;
};