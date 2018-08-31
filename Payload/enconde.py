import bz2, os
path = r'..\\Driver\\Debug\\Driver.sys' 
open('payload.bin', 'wb').write(str(os.path.getsize(path)).rjust(6, '0') + bz2.compress(open(path, 'rb').read()))