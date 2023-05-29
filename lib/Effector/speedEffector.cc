#include "Effector/speedEffector.hh"
#include "utils/justiceNumber.hh"
#include <cmath>
#define EPS 1e-9

template <typename SamplePoint>
static SamplePoint interpolate(const SamplePoint &a, const SamplePoint &b,
                               double ratio) {
  return a + (b - a) * ratio;
}

template <typename SamplePoint>
static void processOnePoint(const EffectorBuf &input, EffectorBuf &transformed,
                            u32 channels, u32 bitsPerSample, double samplePos) {
  u32 bytesPerSample = bitsPerSample / 8;
  for (u32 i = 0; i < channels; i++) {
    SamplePoint sample_channel_left = 0;
    // big-endian, so the first byte is the high byte
    for (u32 j = 0; j < bytesPerSample; j++) {
      sample_channel_left +=
          input[std::floor(bytesPerSample * channels * samplePos +
                           bytesPerSample * i + j)]
          << (8 * (bytesPerSample - j - 1));
    }

    SamplePoint sample_channel_right = 0;
    for (u32 j = 0; j < bytesPerSample; j++) {
      sample_channel_right +=
          input[std::ceil(bytesPerSample * channels * samplePos +
                          bytesPerSample * i + j)]
          << (8 * (bytesPerSample - j - 1));
    }
    SamplePoint sample_channel =
        interpolate<SamplePoint>(sample_channel_left, sample_channel_right,
                                 samplePos - std::floor(samplePos));
    for (u32 j = 0; j < bytesPerSample; j++) {
      transformed.push_back(static_cast<char>(
          (sample_channel >> (8 * (bytesPerSample - j - 1))) & 0xff));
    }
  }
}

static void processOneSample(const EffectorBuf &input, EffectorBuf &transformed,
                             u32 channels, u32 bitsPerSample,
                             double samplePos) {
  switch (bitsPerSample) {
  case 8:
    processOnePoint<u8>(input, transformed, channels, bitsPerSample, samplePos);
    break;
  case 16:
    processOnePoint<u16>(input, transformed, channels, bitsPerSample,
                         samplePos);
    break;
  case 24:
    processOnePoint<u32>(input, transformed, channels, bitsPerSample,
                         samplePos);
    break;
  case 32:
    processOnePoint<u32>(input, transformed, channels, bitsPerSample,
                         samplePos);
    break;
  default:
    fprintf(stderr, "unsupported bits per sample: %d\n", bitsPerSample);
    break;
  }
}

SpeedEffector::SpeedEffector(EffectorOwner effector, double speed,
                             const ALSAHeader &header)
    : header{header}, effector{std::move(effector)}, speed{speed} {}

bool SpeedEffector::next() {
  DRAINRET;
  if (!effector->next()) {
    DRAIN;
  }
  auto &v = effector->getData();
  buf.clear();
  auto new_size = static_cast<size_t>(8 * v.size() / header.channels /
                                      header.bits_per_sample / speed);
  buf.reserve(new_size);
  for (size_t i = 0; i < new_size; i++) {
    processOneSample(v, buf, header.channels, header.bits_per_sample,
                     i * speed);
  }
  return true;
}

bool SpeedEffector::hasData() {
  DRAINRET;
  if (effector->hasData()) {
    return true;
  }
  DRAIN;
}

EffectorBuf &SpeedEffector::getData() { return buf; }