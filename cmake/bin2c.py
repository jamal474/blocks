#!/usr/bin/env python3
"""Convert a binary file to a C++ source file with a byte array.

Usage: bin2c.py <input_file> <variable_name> <output_cpp>

Generates a .cpp containing:
    alignas(16) const unsigned char VAR_NAME[] = { 0x.., ... };
    const std::size_t VAR_NAME_SIZE = sizeof(VAR_NAME);
"""
import os
import sys


def main():
    if len(sys.argv) != 4:
        print(f"Usage: {sys.argv[0]} <input_file> <variable_name> <output_cpp>",
              file=sys.stderr)
        sys.exit(1)

    input_file = sys.argv[1]
    var_name   = sys.argv[2]
    output_cpp = sys.argv[3]

    with open(input_file, 'rb') as f:
        data = f.read()

    basename = os.path.basename(input_file)
    size = len(data)

    with open(output_cpp, 'w') as f:
        f.write(f'// Auto-generated from {basename} ({size} bytes) — DO NOT EDIT\n')
        f.write('#include <cstddef>\n\n')
        f.write(f'alignas(16) extern const unsigned char {var_name}[] = {{\n')

        # Write 16 bytes per line for readability
        for i in range(0, size, 16):
            chunk = data[i:i + 16]
            hex_vals = ','.join(f'0x{b:02x}' for b in chunk)
            f.write(f'    {hex_vals},\n')

        f.write('};\n\n')
        f.write(f'extern const std::size_t {var_name}_SIZE = sizeof({var_name});\n')

    print(f"  Embedded {basename} -> {var_name} ({size} bytes)")


if __name__ == '__main__':
    main()
