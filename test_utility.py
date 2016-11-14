from PIL import Image, ImageDraw
import os, sys


def drawSquare(xOffset, yOffset, color):
    global im_draw, BLOCK_SIZE
    im_draw.rectangle([xOffset * BLOCK_SIZE, yOffset * BLOCK_SIZE, (xOffset + 1) * BLOCK_SIZE, (yOffset + 1) * BLOCK_SIZE], color)


if len(sys.argv) not in (3, 4):
    print("Usage: " + sys.argv[0] + " <path_to_xml_log_file> <cell_size_in_pixels> [-map_only|-theta]")
    exit()

MAP_ONLY = False
THETA = False
if len(sys.argv) == 4:
    if sys.argv[3] == "-map_only":
        MAP_ONLY = True
    elif sys.argv[3] == "-theta":
        THETA = True

BLOCK_SIZE = int(sys.argv[2])
BLACK_COLOR = (0, 0, 0)
WHITE_COLOR = (255, 255, 255)
RED_COLOR = (255, 0, 0)
YELLOW_COLOR = (255, 191, 0)

pathToTestedFile = os.path.abspath(sys.argv[1])
print("Generating from file: '" + pathToTestedFile + "'...")
testedFile = open(pathToTestedFile)

if MAP_ONLY:
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

for i in range(len(mapContainer)):
    for j in range(len(mapContainer[0])):
        if mapContainer[i][j] == '1':
            drawSquare(j, i, BLACK_COLOR)  # SWAP HERE !!!
        elif mapContainer[i][j] == '*':
            drawSquare(j, i, YELLOW_COLOR)  # SWAP HERE !!!
    print((i + 1) / len(mapContainer) * 100, "% generated", sep="")

if MAP_ONLY:
    im.save("test_image.bmp")
    print("Image generated")
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
