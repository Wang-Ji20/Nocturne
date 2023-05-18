// TODO(maybe never): I gave up.
#pragma once
#include "Decoder/abstractDecoder.hh"
#include "Decoder/RawStream.hh"

using RawStreamRef = std::unique_ptr<RawStream>;

struct FLACHeader {
  char magic[4]; // "fLaC"
};

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
  u32 length;
};

// we don't care about other blocks
struct FLACSTREAMHeader {
  u32 minimum_block_size;
  u32 maximum_block_size;
  u32 minimum_frame_size;
  u32 maximum_frame_size;
  u32 sample_rate;
  u32 channels;
  u32 bits_per_sample;
  unsigned long long total_samples;
  char md5[16];
};

struct FLACFrameHeader {
  enum {
    FIXED_BLOCK_SIZE = 0,
    VARIABLE_BLOCK_SIZE = 1,
  } block_size_strategy;
  u32 block_size;
  u32 sample_rate;
  u32 channel_assignment;
  u32 sample_size;
  union{
    u32 sample_number; // for variable block size
    u32 frame_number; // for fixed block size
  };
  u32 crc8;
};

class FLACDecoder : public AbstractDecoder {
public:
  FLACDecoder(std::string_view filename);
  int getData(char *buffer, int size) override;
  const FLACHeader &getFLACHeader() const noexcept { return header_; }
  ~FLACDecoder() {}

private:
  FLACHeader header_{};
  FLACSTREAMHeader streamHeader_{};
  RawStreamRef file_;
};
