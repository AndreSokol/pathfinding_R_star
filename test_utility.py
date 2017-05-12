#!/usr/bin/env python3

from PIL import Image, ImageDraw
import os, sys

def drawSquare(xOffset, yOffset, color):
    global im_draw, BLOCK_SIZE
    im_draw.rectangle([xOffset * BLOCK_SIZE, yOffset * BLOCK_SIZE, (xOffset + 1) * BLOCK_SIZE, (yOffset + 1) * BLOCK_SIZE], color)


if len(sys.argv) not in (3, 4, 5):
    print("Usage: " + sys.argv[0] + " <path_to_xml_file> <cell_size_in_pixels> [-map_only|-theta] [-grid]")
    exit()

MAP_ONLY = False
THETA = False
GRID = False
if len(sys.argv) >= 4:
    if sys.argv[3] == "-map_only":
        MAP_ONLY = True
    elif sys.argv[3] == "-theta":
        THETA = True

if sys.argv[-1] == "-grid":
    GRID = True

BLOCK_SIZE = int(sys.argv[2])
BLACK_COLOR = (0, 0, 0)
WHITE_COLOR = (255, 255, 255)
RED_COLOR = (255, 0, 0)
YELLOW_COLOR = (255, 191, 0)
GRAY_COLOR = (192, 192, 192)

pathToTestedFile = os.path.abspath(sys.argv[1])
print("Generating from file: '" + pathToTestedFile + "'...")
testedFile = open(pathToTestedFile)

if MAP_ONLY:

    start = [];
    finish = [];

    newline = testedFile.readline().strip()
    while newline[0:6] != "<start":
        if newline == "":
            print("Error!")
            exit()
        newline = testedFile.readline().strip()

    start.append(int(newline[8:-9]))
    newline = testedFile.readline().strip()
    start.append(int(newline[8:-9]))

    print("Start point detected:", "(" + str(start[0]) + ", " + str(start[1]) + ")")

    newline = testedFile.readline().strip()
    finish.append(int(newline[9:-10]))
    newline = testedFile.readline().strip()
    finish.append(int(newline[9:-10]))

    print("Finish point detected:", "(" + str(finish[0]) + ", " + str(finish[1]) + ")")

    newline = testedFile.readline().strip()
    while newline != "<grid>":
        if newline == "":
            print("Error!")
            exit()
        newline = testedFile.readline().strip()

    newline = testedFile.readline().strip()
    mapContainer = []
    while newline != "</grid>":
        newline = newline[newline.find(">") + 1:newline.rfind("<")]
        mapContainer.append(newline.split())
        newline = testedFile.readline().strip()

else:
    newline = testedFile.readline().strip()
    while newline != "<path>":
        if newline == "":
            print("No path found!")
            exit()
        newline = testedFile.readline().strip()

    newline = testedFile.readline().strip()
    mapContainer = []
    while newline != "</path>":
        newline = newline[newline.find(">") + 1:newline.rfind("<")]
        mapContainer.append(newline.split())
        newline = testedFile.readline().strip()

im = Image.new("RGB", (len(mapContainer) * BLOCK_SIZE, len(mapContainer[0]) * BLOCK_SIZE), "white")
im_draw = ImageDraw.Draw(im)

if GRID:
    for i in range(1, len(mapContainer)):
        im_draw.rectangle([i * BLOCK_SIZE, 0, i * BLOCK_SIZE + 1, len(mapContainer[0]) * BLOCK_SIZE], GRAY_COLOR)
    for j in range(1, len(mapContainer[0])):
        im_draw.rectangle([0, j * BLOCK_SIZE, len(mapContainer) * BLOCK_SIZE, j * BLOCK_SIZE + 1], GRAY_COLOR)

for i in range(len(mapContainer)):
    for j in range(len(mapContainer[0])):
        if mapContainer[i][j] == '1':
            drawSquare(j, i, BLACK_COLOR)  # SWAP HERE !!!
        elif mapContainer[i][j] == '*':
            drawSquare(j, i, YELLOW_COLOR)  # SWAP HERE !!!
    print((i + 1) / len(mapContainer) * 100, "% generated", sep="")

if MAP_ONLY:
    drawSquare(start[0], start[1], RED_COLOR)
    drawSquare(finish[0], finish[1], YELLOW_COLOR)

    im.save("test_image.bmp")
    print("Image generated\nStart is RED, finish is YELLOW")
    exit()

while newline != "<hplevel>":
    newline = testedFile.readline().strip()

newline = testedFile.readline().strip()

# draw start point
input_list = newline.split('"')
drawSquare(int(input_list[3]), int(input_list[5]), RED_COLOR)
hops = [((int(input_list[3]) * BLOCK_SIZE + BLOCK_SIZE // 2,
          int(input_list[5]) * BLOCK_SIZE + BLOCK_SIZE // 2))]


while newline != "</hplevel>":
    input_list = newline.split('"')
    drawSquare(int(input_list[7]), int(input_list[9]), RED_COLOR)
    hops.append((int(input_list[7]) * BLOCK_SIZE + BLOCK_SIZE // 2,
                 int(input_list[9]) * BLOCK_SIZE + BLOCK_SIZE // 2))
    newline = testedFile.readline().strip()

if THETA:
    im_draw.line(hops, YELLOW_COLOR, (BLOCK_SIZE + 9) // 10)

im.save("test_image.bmp")

print("Image saved!")
