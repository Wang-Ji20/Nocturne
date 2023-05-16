// Nocturne 2023
// Identification: tools/tty_printer/main.cc

#include "Decoder/WAVDecoder.hh"
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <unistd.h>

// return the string representation of a WAVHeader
std::string headerToString(const WAVHeader &header) {
  std::string str =
      "Header Length: " + std::to_string(sizeof(WAVHeader)) + "\n";
  str += "RIFF: " + std::string(header.riff, 4) + "\n";
  str += "Size: " + std::to_string(header.size) + "\n";
  str += "WAVE: " + std::string(header.wave, 4) + "\n";
  str += "fmt : " + std::string(header.fmt, 4) + "\n";
  str += "fmt_size: " + std::to_string(header.fmt_size) + "\n";
  str += "format: " + std::to_string(header.format) + "\n";
  str += "channels: " + std::to_string(header.channels) + "\n";
  str += "sample_rate: " + std::to_string(header.sample_rate) + "\n";
  str += "byte_rate: " + std::to_string(header.byte_rate) + "\n";
  str += "block_align: " + std::to_string(header.block_align) + "\n";
  str += "bits_per_sample: " + std::to_string(header.bits_per_sample) + "\n";
  return str;
}

void writeToFile(const char *filename, const std::string &content) {
  int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    throw std::runtime_error(strerror(errno));
  }
  if (write(fd, content.c_str(), content.size()) == -1) {
    throw std::runtime_error(strerror(errno));
  }
  close(fd);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <wav file>" << std::endl;
    return 1;
  }
  try {
    WAVDecoder decoder(argv[1]);
    std::string str = headerToString(decoder.getWAVHeader());
    std::cout << str;
    writeToFile("wav_header.txt", str);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}