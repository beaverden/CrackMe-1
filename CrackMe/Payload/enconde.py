import bz2, os
os.chdir(os.path.dirname(__file__))
path = r'..\\Debug\\Driver.sys' 
binary_data = str(os.path.getsize(path)).rjust(6, '0') + bz2.compress(open(path, 'rb').read())

template = """
#pragma once
const BYTE payload[] = {%s};
"""
header = template % ','.join(['0x%02X' % ord(val) for val in binary_data])
with open(r'..\\CrackMe\\payload.h', 'wb') as header_file:
	header_file.write(header)
with open('payload.bin', 'wb') as payload_bin:
	payload_bin.write(binary_data)