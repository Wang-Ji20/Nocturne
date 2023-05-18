// decodes raw data from a file, bit by bit
// Nocturne 2023

#pragma once

#include "utils/justiceNumber.hh"
#include "utils/ruskell.hh"

#include <fstream>
#include <memory>

using fileRef = std::unique_ptr<std::fstream>;

class RawStream {
public:
  RawStream(fileRef file, bool is_bigendian = true, size_t offset = 0)
      : file_{std::move(file)}, big_endian_{is_bigendian}, current_byte_{
                                                               file_->get()} {}

  RawStream(const RawStream &) = delete;
  RawStream(RawStream &&) = delete;
  RawStream &operator=(const RawStream &) = delete;
  RawStream &operator=(RawStream &&) = delete;

  ~RawStream() { file_->close(); }

  [[nodiscard]] Maybe<u64> getBit(u8 bits_num);
  int read(char *buffer, int size);

  void seekg(u64 seekLen, std::_Ios_Seekdir seekdir);

private:
  [[nodiscard]] Maybe<u64> getBitBE(u8 bits_num);
  [[nodiscard]] Maybe<u64> getBitLE(u8 bits_num);

  fileRef file_{};
  bool big_endian_{true};
  int current_bit_ {0};
  int current_byte_{0};
};