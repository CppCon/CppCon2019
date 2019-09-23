#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include <png.h>

class PngWriter {
  std::unique_ptr<FILE, decltype(fclose) *> file_;
  png_structp pngStruct_{};
  png_infop info_{};

public:
  PngWriter(const char *filename, int width, int height);
  ~PngWriter();

  PngWriter(const PngWriter &) = delete;
  PngWriter &operator=(const PngWriter &) = delete;
  PngWriter(PngWriter &&) = delete;
  PngWriter &operator=(PngWriter &&) = delete;

  void addRow(const uint8_t *rowData);

  [[nodiscard]] bool ok() const { return pngStruct_ && info_; }
};
