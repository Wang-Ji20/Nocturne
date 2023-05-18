#include "Decoder/FLACDecoder.hh"

static FLACSTREAMHeader parseStreamHeader(RawStream &file) {
  FLACSTREAMHeader header{};
  // read 16 bits
  header.minimum_block_size = file.getBit(16).value();
  header.maximum_block_size = file.getBit(16).value();
  header.minimum_frame_size = file.getBit(24).value();
  header.maximum_frame_size = file.getBit(24).value();
  header.sample_rate = file.getBit(20).value();
  header.channels = file.getBit(3).value() + 1;
  header.bits_per_sample = file.getBit(5).value() + 1;
  header.total_samples = file.getBit(36).value();
  file.read(header.md5, 16);
  return header;
}

static FLACMetadataBlockHeader parseMetadataBlockHeader(RawStream &file) {
  FLACMetadataBlockHeader header{};
  header.last_block_flag = file.getBit(1).value()
                               ? FLACMetadataBlockHeader::LAST_BLOCK
                               : FLACMetadataBlockHeader::NOT_LAST_BLOCK;
  header.block_type = file.getBit(7).value()
                          ? FLACMetadataBlockHeader::INVALID
                          : FLACMetadataBlockHeader::STREAMINFO;
  header.length = file.getBit(24).value();
  return header;
}

FLACDecoder::FLACDecoder(std::string_view filename) {
  fileRef fstream = std::make_unique<std::fstream>();
  fstream->open(filename.data(), std::ios::in | std::ios::binary);
  if (!fstream->is_open()) {
    throw std::runtime_error("Failed to open file");
  }

  fstream->read(reinterpret_cast<char *>(&header_), sizeof(header_));
  if (std::string_view(header_.magic, 4) != "fLaC") {
    throw std::runtime_error("Not a FLAC file");
  }

  file_ = std::make_unique<RawStream>(std::move(fstream));
  FLACMetadataBlockHeader blockHeader;
  do {
    blockHeader = parseMetadataBlockHeader(*file_);
    if (blockHeader.block_type == FLACMetadataBlockHeader::STREAMINFO) {
      streamHeader_ = parseStreamHeader(*file_);
    } else {
      file_->seekg(blockHeader.length, std::ios::cur);
    }
  } while (blockHeader.last_block_flag == 0);

  alsaHeader = ALSAHeader{
      .sample_rate = streamHeader_.sample_rate,
      .channels = static_cast<unsigned short>(streamHeader_.channels),
      .bits_per_sample =
          static_cast<unsigned short>(streamHeader_.bits_per_sample),
  };
}

[[nodiscard]] auto FLACDecoder::getData(char *buffer, int size) -> int {
  return file_->read(buffer, size);
}