/*
 * © 2020 Michael Percival <m@michaelpercival.xyz>
 *   See LICENSE file for copyright and license details.
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define PNG_DEBUG 3
#include <png.h>

#include "png_writer.hpp"

void PngWriter::write_png_file(std::string file_name, int width, int height, std::byte* row_pointers) {
    png_structp png_ptr;
    png_infop info_ptr;

    /* create file */
    FILE *fp = fopen(file_name.c_str(), "wb");

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    setjmp(png_jmpbuf(png_ptr));
    png_init_io(png_ptr, fp);

    setjmp(png_jmpbuf(png_ptr));

    png_set_IHDR(png_ptr, info_ptr, width, height,
             16, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
             PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);

    setjmp(png_jmpbuf(png_ptr));

    png_write_image(png_ptr, (png_bytep *)row_pointers);

    setjmp(png_jmpbuf(png_ptr));

    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
    //for (uint32_t y = 0; y < height; y++)
    //    free(row_pointers[y]);

    //free(row_pointers);

    fclose(fp);
}
