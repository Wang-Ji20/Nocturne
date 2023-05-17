// TODO(maybe never): I gave up.
#pragma once
#include "Decoder/Decoder.hh"

struct FLACHeader {
  char magic[4]; // "fLaC"
} __attribute__((packed));

struct FLACMetadataBlockHeader {
  enum {
    LAST_BLOCK = 1,
    NOT_LAST_BLOCK = 0,
  } last_block_flag;
  enum {
    STREAMINFO = 0,
    PADDING = 1,
    APPLICATION = 2,
    SEEKTABLE = 3,
    VORBIS_COMMENT = 4,
    CUESHEET = 5,
    PICTURE = 6,
    INVALID = 127,
  } block_type;
  unsigned int length;
};

// we don't care about other blocks
struct FLACSTREAMHeader {
  unsigned int minimum_block_size;
  unsigned int maximum_block_size;
  unsigned int minimum_frame_size;
  unsigned int maximum_frame_size;
  unsigned int sample_rate;
  unsigned int channels;
  unsigned int bits_per_sample;
  unsigned long long total_samples;
  char md5[16];
};

class FLACDecoder : public Decoder {
public:
  FLACDecoder(std::string_view filename);
  int getData(char *buffer, int size) override;
  const FLACHeader &getFLACHeader() const noexcept { return header; }

private:
    FLACHeader header{};
    FLACSTREAMHeader streamHeader{};
};
