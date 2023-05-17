#include "Decoder/FLACDecoder.hh"
#include "utils/APInt.hh"

static FLACSTREAMHeader parseStreamHeader(std::fstream &file) {
  FLACSTREAMHeader header{};
  char buf[32];
  // read 16 bits
  file.read(buf, sizeof(short));
  header.minimum_block_size = bitsToNum(buf, 16, false);
  file.read(buf, sizeof(short));
  header.maximum_block_size = bitsToNum(buf, 16, false);
  file.read(buf, 3);
  header.minimum_frame_size = bitsToNum(buf, 24, false);
  file.read(buf, 3);
  header.maximum_frame_size = bitsToNum(buf, 24, false);
  file.read(buf, 3);
  header.sample_rate = bitsToNum(buf, 20, false);
  header.channels = ((buf[2] & 0x0E) >> 1) + 1;
  header.bits_per_sample = (buf[2] & 0x01) << 4;
  file.read(buf, 1);
  header.bits_per_sample |= (buf[0] & 0xF0) >> 4;
  header.bits_per_sample++;
  header.total_samples = ((uint64_t)(buf[0] & 0x0F)) << 32;
  file.read(buf, 4);
  header.total_samples |= bitsToNum(buf, 32, false);
  file.read(header.md5, 16);
  return header;
}

static FLACMetadataBlockHeader parseMetadataBlockHeader(std::fstream &file) {
  FLACMetadataBlockHeader header{};
  char buf[4];
  file.read(buf, 1);
  header.last_block_flag = ((buf[0] & 0x80) >> 7) ? FLACMetadataBlockHeader::LAST_BLOCK
                           : FLACMetadataBlockHeader::NOT_LAST_BLOCK;
  header.block_type = (buf[0] & 0x7F) ? FLACMetadataBlockHeader::INVALID
                      : FLACMetadataBlockHeader::STREAMINFO;
  file.read(buf, 3);
  header.length = bitsToNum(buf, 24, false);
  return header;
}

FLACDecoder::FLACDecoder(std::string_view filename) {
  file.open(filename.data(), std::ios::in | std::ios::binary);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file");
  }

  file.read(reinterpret_cast<char *>(&header), sizeof(header));
  if (std::string_view(header.magic, 4) != "fLaC") {
    throw std::runtime_error("Not a FLAC file");
  }

  FLACMetadataBlockHeader blockHeader;
  do {
    blockHeader = parseMetadataBlockHeader(file);
    if (blockHeader.block_type == FLACMetadataBlockHeader::STREAMINFO) {
      streamHeader = parseStreamHeader(file);
    } else {
      file.seekg(blockHeader.length, std::ios::cur);
    }
  } while (blockHeader.last_block_flag == 0);

  data_offset = file.tellg();
  alsaHeader = ALSAHeader {
      .sample_rate = streamHeader.sample_rate,
      .channels = static_cast<unsigned short>(streamHeader.channels),
      .bits_per_sample = static_cast<unsigned short>(streamHeader.bits_per_sample),
  };
}

[[nodiscard]] auto
FLACDecoder::getData(char *buffer, int size) -> int {
  file.read(buffer, size);
  return file.gcount();
}