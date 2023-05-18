#include "Decoder/RawStream.hh"
#include <stdexcept>

Maybe<u64> RawStream::getBit(u8 bits_num) {
  if (bits_num > 64) {
    throw std::runtime_error("bits num should < 64");
  }
  if (big_endian_)
    return getBitBE(bits_num);
  return getBitLE(bits_num);
}

// big-endian get bit
Maybe<u64> RawStream::getBitBE(u8 bits_num) {
  u64 result = 0;
  for (int i = 0; i < bits_num; i++) {
    if (current_byte_ == EOF) {
      return std::nullopt;
    }
    result |= ((current_byte_ >> (7 - current_bit_)) & 1) << (bits_num - i - 1);
    current_bit_ = (current_bit_ + 1) % 8;
    if (current_bit_ == 7) {
      current_byte_ = file_->get();
    }
  }
  return result;
}

Maybe<u64> RawStream::getBitLE(u8 bits_num) {
  throw std::runtime_error("not implemented yet");
}

void RawStream::seekg(u64 seekLen, std::_Ios_Seekdir seekdir) {
  file_->seekg(seekLen - 1, seekdir);
  current_byte_ = file_->get();
  current_bit_ = 0;
}

int RawStream::read(char *buffer, int size) {
  current_bit_ = 0;
  buffer[0] = current_byte_;
  file_->read(&buffer[1], --size);
  return file_->gcount() + 1;
}