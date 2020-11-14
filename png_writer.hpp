/*
 * © 2020 Michael Percival <m@michaelpercival.xyz>
 *   See LICENSE file for copyright and license details.
 */

#include <string>

#include <png.h>

class PngWriter {
public:
    static void write_png_file(std::string file_name, int width, int height, png_bytep* row_pointers);
};
