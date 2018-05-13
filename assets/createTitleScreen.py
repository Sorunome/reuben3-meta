#!/usr/bin/python3
from PIL import Image
import sys, struct, subprocess

IN = "fita.bmp"
WIDTH = 80
HEIGHT = 64
OUT = "fita.raw"

im = Image.open(IN)
pix = im.load()
print("Size:", im.size)

frames = int(im.size[1] / HEIGHT)

print("Frames:", frames)

pix_colors = []

for fr in range(frames):
	for y in range(HEIGHT):
		for x in range(WIDTH):
			p = pix[x, y + fr*HEIGHT]
			if not p in pix_colors:
				pix_colors.append(p)
print("Colors:", pix_colors)
if len(pix_colors) > 3:
	print("Too many colors")
	sys.exit(-1)

def compress(b):
	with open('tmpin', 'wb+') as f:
		f.write(b)
	subprocess.call(['./apack', 'c', 'tmpin', 'tmpout'])
	bb = b''
	with open('tmpout', 'rb') as f:
		bb = f.read()
	return bb

with open(OUT, 'wb+') as f:
	bytesFramePrev = 0
	for fr in range(frames):
		byteArray = []
		byte = 0
		
		for y in range(HEIGHT):
			for x in range(WIDTH):
				p = [0xD, 1, 0][pix_colors.index(pix[x, y + fr*HEIGHT])]
				if x%2:
					byte |= p
					byteArray.append(byte)
				else:
					byte = p << 4
		compressed = compress(bytes(byteArray))
		f.write(struct.pack("<H", len(compressed)))
		f.write(compressed)
