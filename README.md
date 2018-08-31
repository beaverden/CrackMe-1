# CrackMe #1

All the provided code is and will be only a PoC, never intended to cause any harm, use with caution.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


## Walkthrough

Everything starts in `crackme.cpp` in the function `main`. We see some resource manipulation `FindResource`, `SizeofResource`, `LoadResource`, `LockResource`.
Then, the supposed resource (spoiler alert, there is only one, a bitmap) is dropped to the disk. Is that all?

Of course not. Notice the line `*(int*)(0) = ':un:';` that deferences NULL pointer, which will never be allowed and will generate an exception.
Let's return to the start of the function and look at this code
 
```
    push offset ExceptionHandler
    push fs:[0x0]
    mov fs:[0x0], esp

```

Right! It's pushing an exception handler to the SEH linked list. So we now have a trace of what happens when we derefence the pointer - the execution jumps to  `ExceptionHandler`.
In here, we find 

```

	call $ + 5;
	pop eax
	add eax, 0x100
	_loop:
	add eax, 1
	mov ebx, dword ptr [eax]
	cmp ebx, ':un:'
	jnz _loop
	add eax, 4
	mov jmpBack, eax

```

This code finds the point at which the exception was thrown (the NULL pointer dereference) and sets the global variable `jmpBack` to the next instruction. Obviously, this is done so we could get back inside the `main` function after we finish our tricks.

After that, we see this code, which is obviously a way to call the `Decrypt` function
```
    push Decrypt
    ret
    push 0
    call ExitProcess
```

The `Decrypt` function does what it's supposed to do, decrypts the payload and some important functions. It's pretty 
straight forward, it starts with the image base (in our case, we use less than 4 bytes of addresses for code, so we can freely just strip them to get the image base), adds 0x1000 to reach the first section, then does `ROL5` to each byte until it finds the marker

```
    mov eax, 0x13371337 // Marker 
    call $+5
    pop eax
    and eax, 0xFFFF0000
    add eax, 0x1000
    dec eax
    _loop:
        inc eax
        mov ebx, dword ptr [eax]
        cmp ebx, 0x13371337 // Search for marker
        je _endloop

        mov bl, byte ptr [eax]
        rol bl, 5
        mov byte ptr [eax], bl
        jne _loop     
    _endloop:
```

After decryption, we're calling the `DecodePayload` function, which is where the fun starts.

At first, it drops a file to the disk, called `snake`, which turns out to be an obfuscated `.pyc`
Let's download [uncompyle6](https://pypi.python.org/pypi/uncompyle6) to get the original source as a `.py`
```
def cc(s):
    return ''.join([chr(ord(x) ^ 0x50) for x in s])

fuck = __import__(cc('2*b'))
your = __import__(cc('#)#'))
strings = getattr(getattr(your, cc('#$4?%$')), cc('\'"9$5'))
dude = getattr(getattr(your, cc('#$49>')), cc('"514'))()
strings(getattr(fuck, cc('453?= "5##'))(dude[6:])[:int(dude[:6])])
```
The obfuscation is quite simple, the _encrypted_ strings are just `xor`-ed with `0x50` and the `getattr` method is just a fancy way of getting a function from an object by its name. For example, get the `write` method out of `sys.stdin`.

The format is: first 6 bytes are the original (uncompressed) payload length, all what follows is bz2 compressed data
```
import bz2
import sys
data = sys.stdin.read()
# The trick with the length is not really needed in here, but needed when we drop the file to the disk
data_length = int(data[:6])
result = bz2.decompress(data[6:])[:data_length]
sys.stdout.write(result)
```

So now when we solved the script, what happens with it?
1. We create two pipes with `CreatePipe`, one for `stdin` and the other for `stdout`
2. Then we `CreateProcess("C:\\Python27\\python.exe -u snake", ...)` with `stdin` and `stdout` redirected to pipes.
3. We write some data to `stdin` and then read some data from `stdout`.
4. Obviously, we feed some data to the `bz2 decoder` though interprocess communication.
5. The data being fed is insinde `payload.h` and is a `bz2` encoded `Native Kernel Driver`.
6. The original uncompressed driver is located inside `Build/Driver.sys`
7. We drop the driver inside `%temp%/sunca`, which is romanian for `ham` :)
8. Using `OpenSCManager`, `OpenService`, `CreateService`, `StartService` we create and start a service for our driver called `Salam` which is romanian for `salami` :) 
9. After loading, we wait 30 seconds
10. Then we try to read a suspicious file `C:\\suc` (supposedly created by the driver) and then try to decrypt the resource we got with `LockResource` using the contents of the file as `RC4` key. (Preventively `VirtualProtect`-ing the resource, which is `readonly`

We then return to the main function and drop the resource under the name `sefu labani.bmp`.

So, it remains to find out what this `salam` driver does and what it writes to the `C:\\suc` file

