#!/usr/bin/env python3
"""Generate a .ico file from a PNG — no external dependencies.

Usage: gen_ico.py <input_png> <output_ico>

The ICO format supports PNG-compressed entries (Windows Vista+).
We wrap the source PNG directly as a single 256x256 entry.
Windows scales it automatically for taskbar / Explorer display.
"""
import struct
import sys


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_png> <output_ico>", file=sys.stderr)
        sys.exit(1)

    input_png  = sys.argv[1]
    output_ico = sys.argv[2]

    with open(input_png, 'rb') as f:
        png_data = f.read()

    # Verify PNG signature
    if png_data[:8] != b'\x89PNG\r\n\x1a\n':
        print("Error: Input file is not a valid PNG", file=sys.stderr)
        sys.exit(1)

    # Parse dimensions from the IHDR chunk (bytes 16-23 of a PNG)
    width  = struct.unpack('>I', png_data[16:20])[0]
    height = struct.unpack('>I', png_data[20:24])[0]

    # ICO directory entries use 0 to represent 256
    ico_w = width  if width  < 256 else 0
    ico_h = height if height < 256 else 0

    with open(output_ico, 'wb') as f:
        # ICO header: reserved(2) + type=1(2) + image_count=1(2)
        f.write(struct.pack('<HHH', 0, 1, 1))

        # Directory entry (16 bytes):
        #   width, height, color_count, reserved,
        #   color_planes, bits_per_pixel, image_size, data_offset
        data_offset = 6 + 16  # header(6) + one directory entry(16)
        f.write(struct.pack('<BBBBHHII',
                            ico_w, ico_h,
                            0,              # color count (0 = no palette)
                            0,              # reserved
                            1,              # color planes
                            32,             # bits per pixel
                            len(png_data),  # size of PNG data
                            data_offset))

        # Raw PNG data
        f.write(png_data)

    print(f"  Generated {output_ico} ({width}x{height}, {len(png_data)} bytes)")


if __name__ == '__main__':
    main()
