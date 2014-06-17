#!/usr/bin/python

# Re-Multiplexer for SVC-DASH. Reorders SVC layer chunks into a single SVC bitstream.

# (1:stream_out, 2:bl0_in, 3:el1_in, 4:el2_in, ...)

# Note: Does currently not work with input pipes because we cannot detect the actual end of the stream (in contrast to temporary silence). 
# TODO: Support unix domain sockets, which should enable us to detect shutdown of the sender.

'''
The MIT License

Copyright (c) 2013 Michael Grafl

Permission is hereby granted, free of charge, to any person obtaining a copy 
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
'''

# This work was supported in part by the EC in the context of the ALICANTE project (FP7-ICT-248652). http://www.ict-alicante.eu/

import sys, os, struct;

if(len(sys.argv) < 2):
    print("Usage: \n  python", sys.argv[0], "stream_out bl0_in el1_in el2_in ...")
    quit()

sep = struct.pack("BBBB", 0, 0, 0, 1)
nLayers = len(sys.argv) - 2
naluStreams = [None]*nLayers
positions = [None]*nLayers

for i in range(nLayers):
    with open(sys.argv[i+2], 'rb') as fpIn:
        naluStreams[i] = fpIn.read().split(sep)[1:]
        positions[i] = 0

with open(sys.argv[1], 'wb') as fpOut:
    active = True
    while active:
        active = False
        
        for i in range(nLayers):
            eos, found = False, False
            
            while ((not eos) and (not found)): # copy NALUs until we find type 1, 5, or 20.
                pos = positions[i]
                
                if (pos >= len(naluStreams[i])):
                    eos = True
                
                else:
                    active = True
                    n = naluStreams[i][pos]
                    
                    if (len(n) > 0): # NALU long enough
                        hdr = struct.unpack_from("B", n)
                        naluType = hdr[0] & 0x1f
                        
                        if (naluType in [1, 5, 20]): # EL
                            found = True
                        
                    fpOut.write(sep+n)
                    positions[i]+= 1
