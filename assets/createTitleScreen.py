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
	byteArray = []
	for fr in range(frames):
		bytesFrame = 0
		byte = 0
		byteIndex = 4
		curColor = 0
		curCount = 0
		def addSlot(num):
			global byte, byteIndex, bytesFrame, byteArray
			byteIndex -= 1
			byte |= num << (2*(3-byteIndex))
			if byteIndex == 0:
				byteArray.append(byte)
				byte = 0
				byteIndex = 4
				bytesFrame += 1
		def flush():
			global byte, byteIndex, bytesFrame, byteArray
			if byteIndex < 4:
				byteArray.append(byte)
				byte = 0
				byteIndex = 4
				bytesFrame += 1
		def writePixel():
			global curCount
			if curCount < 4:
				addSlot(curCount-1)
				addSlot(curColor)
				curCount = 0
				return
			addSlot(3)
			curCount -= 1
			curCountLower = curCount & 0b11
			curCountUpper = (curCount & 0b1100) >> 2
			addSlot(curCountLower)
			addSlot(curCountUpper)
			addSlot(curColor)
			curCount = 0
				
		for y in range(HEIGHT):
			for x in range(WIDTH):
				p = pix_colors.index(pix[x, y + fr*HEIGHT])
				if curCount > 0 and p == curColor and curCount < 16:
					curCount += 1
					continue
				elif curCount > 0:
					writePixel()
				
				curColor = p
				curCount = 1
		writePixel()
		flush();
		if not fr % 2:
			print(fr, bytesFrame, bytesFramePrev + bytesFrame)
			if bytesFramePrev + bytesFrame > 2048:
				print("WON'T WORK!!!! BLOCK TOO LARGE")
				sys.exit(-1)
			compressed = compress(bytes(byteArray))
			f.write(struct.pack("<H", len(compressed)))
			f.write(compressed)
			byteArray = []
		bytesFramePrev = bytesFrame
