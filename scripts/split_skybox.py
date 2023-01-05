#!/usr/bin/env python

# Will split a png cubemap/skymap image produced by blender into 6 separated image files for use in a skybox within unity
# Requires Python Imaging Library > http://www.pythonware.com/products/pil/

# The author takes no responsibility for any damage this script might cause,
# feel free to use, change and or share this script.
# 2013-07, CanOfColliders, m@canofcolliders.com

from PIL import Image
import sys, os

path = os.path.abspath("") + "/"
processed = False

def processImage(path, name):
	img = Image.open(os.path.join(path, name))
	size_x = img.size[0] / 4 # splits the width of the image by 3, expecting the 3x2 layout blender produces.
	size_y = img.size[1] / 3
	splitAndSave(img, size_x, 		size_y, 	size_x, size_y, addToFilename(name, "_front_X+"))
	splitAndSave(img, size_x * 3, 	size_y, 	size_x, size_y, addToFilename(name, "_back_X-"))
	splitAndSave(img, size_x * 2, 	size_y, 	size_x, size_y, addToFilename(name, "_right_Y-"))
	splitAndSave(img, 0, 			size_y, 	size_x, size_y, addToFilename(name, "_left_Y+"))
	splitAndSave(img, size_x, 		size_y * 2,	size_x, size_y, addToFilename(name, "_bottom_Z-"))
	splitAndSave(img, size_x, 		0, 			size_x, size_y, addToFilename(name, "_top_Z+"))

def addToFilename(name, add):
	name = name.split('.')
	return name[0] + add + "." + "hdr" #name[1]

def splitAndSave(img, startX, startY, size_x, size_y, name):
	area = (startX, startY, startX + size_x, startY + size_y)
	saveImage(img.crop(area), path, name)

def saveImage(img, path, name):
	try:
		img.save(os.path.join(path, name))
	except:
		print("*   ERROR: Could not convert image.")
		pass

for arg in sys.argv:
	if ".png" in arg or ".jpg" in arg:
		processImage(path, arg)
		processed = True

if not processed:
	print("*  ERROR: No Image")
	print("   usage: 'python script.py image-name.png'")
