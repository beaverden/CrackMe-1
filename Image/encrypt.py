from rc4 import * 
import struct

image = open('buce.bmp', 'rb').read()
offset = struct.unpack('<I', image[10:14])[0]
width = struct.unpack('<I', image[18:22])[0]
height = struct.unpack('<I', image[22:26])[0]

array = bytearray(image[offset:offset+width*height])
result = image[:offset] + rc4(array, 'sugi.pl')

open('buce_decrypted.bmp', 'wb').write(result)
