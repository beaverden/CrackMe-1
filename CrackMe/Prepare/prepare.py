import pefile
import sys
import struct
import os

def ROR(x, n):
    mask = (2L**n) - 1
    mask_bits = x & mask
    return (x >> n) | (mask_bits << (8 - n))

pe =  pefile.PE(sys.argv[1])
pe.sections[0].Name = 'Crack #1'
pe.sections[1].Name = 'Zero? :('
pe.sections[2].Name = 'Imports?'
pe.sections[3].Name = 'DRS 0_0'

IMAGE_SCN_MEM_WRITE = 0x80000000
pe.sections[0].Characteristics |= IMAGE_SCN_MEM_WRITE

out_file = os.path.join(os.path.dirname(sys.argv[1]), 'CrackMe #1.exe$')
pe.write(out_file)
pe.close()

data = bytearray(open(out_file, 'rb').read())
data = data[:0x4E] + 'Would it run in a Windows 7 VM?'.ljust(39, ' ') + data[0x4E+39:]

ptr = pe.sections[0].PointerToRawData
print ptr
done = False
while True:
    if struct.unpack('<I', data[ptr:ptr+4])[0] == 0x83EC8B55:
        print 'Function'
        for x in range(ptr, ptr+40):
            if struct.unpack('<I', data[x:x+4])[0] == 0x13371337:
                done = True
                break
    if done: break
    if ''.join([chr(x) for x in data[ptr:ptr+28]]) == 'Unhandled exception occurred':
        print 'Found exception'
        ptr += 28
        continue
    data[ptr] = ROR(data[ptr], 5)
    ptr += 1

open(out_file, 'wb').write(data)
    
