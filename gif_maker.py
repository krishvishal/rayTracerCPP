import imageio
import os
from natsort import natsort


def create_gif(filenames, duration):
    images = []
    for filename in filenames:
        imagees.append(imageio.imread(filename))
    output_file = 'raytrace_2.gif'
    imageio.mimsave(output_file, images, duration=duration)


path = '/home/vkvishal/Documents/RayTracing/' 

filenames = []
for file in os.listdir(path):
    # filename = os.fsdecode(file)
    if file.endswith(('.bmp')):
        filenames.append(file)
filenames = natsort.natsorted(filenames)
create_gif(filenames, duration=5)
