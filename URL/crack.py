import zlib
import string
import itertools
url = 'sugi.pl'
crc = zlib.crc32(url) & 0xFFFFFFFF
print hex(crc)

values = string.lowercase + '.'
t = 0
for s in itertools.product(values, repeat=6):
	k = ''.join(s) + 'l'
	#print k
	t += 1
	if t % 1000000 == 0:
		print k
		#print k
	if zlib.crc32(k) == crc:
		print 'CRC', k