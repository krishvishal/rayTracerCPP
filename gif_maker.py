# import imageio
# import os
# from natsort import natsort

# path = '/home/vkvishal/Documents/RayTracing/'  # on Mac: right click on a folder, hold down option, and click "copy as pathname"

# #image_folder = os.fsencode(path)

# filenames = []

# for file in os.listdir(path):
#     # filename = os.fsdecode(file)
#     if file.endswith(('.bmp')):
#         filenames.append(file)
# print('1')
# filenames = natsort.natsorted(filenames)  # this iteration technique has no built in order, so sort the frames

# images = list(map(lambda filename: imageio.imread(filename), filenames))
# print('2')
# imageio.mimsave(os.path.join(path, 'movie.gif'), images, format='GIF', duration=4)
# print('3')
import imageio
import os
from natsort import natsort


def create_gif(filenames, duration):
    images = []
    for filename in filenames:
        imagees.append(imageio.imread(filename))
    output_file = 'raytrace_2.gif'
    imageio.mimsave(output_file, images, duration=duration)


path = '/home/vkvishal/Documents/RayTracing/'  # on Mac: right click on a folder, hold down option, and click "copy as pathname"

#image_folder = os.fsencode(path)

filenames = []
print('1')
for file in os.listdir(path):
    # filename = os.fsdecode(file)
    if file.endswith(('.bmp')):
        filenames.append(file)
print('2')
filenames = natsort.natsorted(filenames)
create_gif(filenames, duration=5)
print('3')
