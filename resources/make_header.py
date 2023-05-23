import sys
import os.path

# Command line arguments
header_file = sys.argv[1]
input_files = sys.argv[2].split(";")

# Create out file
outf = open(header_file, "w+")

# Preamble
header_escaped = os.path.basename(header_file).replace(".", "_")

outf.write('#ifndef _{}_\n'.format(header_escaped))
outf.write('#define _{}_\n'.format(header_escaped))
outf.write('\n')
outf.write('#ifdef __cplusplus\n')
outf.write('extern "C" {\n')
outf.write('#endif\n')
outf.write('\n')

# Generate symbol names
for input in input_files:
    outf.write('// Auto-generated from {}\n'.format(input))

    input_escaped = input.replace(".", "_")
    outf.write('extern const char* _binary_{}_start;\n'.format(input_escaped))
    outf.write('extern const char* _binary_{}_end;\n'.format(input_escaped))
    outf.write('\n')

# Postamble
outf.write('#ifdef __cplusplus\n')
outf.write('}\n')
outf.write('#endif\n')
outf.write('\n')
outf.write('#endif // _{}_\n'.format(header_escaped))

outf.close()