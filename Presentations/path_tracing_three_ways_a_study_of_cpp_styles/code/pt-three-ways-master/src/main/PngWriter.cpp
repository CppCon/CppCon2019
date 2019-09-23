#include "PngWriter.h"

PngWriter::PngWriter(const char *filename, int width, int height)
    : file_(fopen(filename, "wb"), fclose) {
  if (!file_)
    return;
  pngStruct_ =
      png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

  if (!pngStruct_)
    return;

  info_ = png_create_info_struct(pngStruct_);
  if (!info_)
    return;

  png_init_io(pngStruct_, file_.get());

  png_set_IHDR(pngStruct_, info_, width, height, 8, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
               PNG_FILTER_TYPE_BASE);

  png_write_info(pngStruct_, info_);
}

PngWriter::~PngWriter() {
  png_write_end(pngStruct_, info_);
  png_destroy_write_struct(&pngStruct_, &info_);
}

void PngWriter::addRow(const uint8_t *rowData) {
  const png_byte *rowPointers[] = {reinterpret_cast<const png_byte *>(rowData)};
  png_write_rows(pngStruct_, const_cast<png_byte **>(rowPointers), 1);
}
