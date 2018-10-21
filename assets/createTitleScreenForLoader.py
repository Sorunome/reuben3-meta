#!/usr/bin/python3
from PIL import Image
import sys, struct, subprocess

IN = "fita.bmp"
LOGO = "title.png"
VERSION = "titlescreen_version.png"
WIDTH = 80
HEIGHT = 64
LOGO_X = 2
LOGO_Y = 21
VERSION_X = 1
VERSION_Y = 64-6-1
OUT = "TITLESCREEN.BMP"

im = Image.open(IN)
logo = Image.open(LOGO)
version = Image.open(VERSION)
print("Size:", im.size)

frames = int(im.size[1] / HEIGHT)

print("Frames:", frames)

for fr in range(frames):
	im.paste(logo, (LOGO_X, LOGO_Y + fr*HEIGHT), logo)
	im.paste(version, (VERSION_X, VERSION_Y + fr*HEIGHT), version)

im.save(OUT)
