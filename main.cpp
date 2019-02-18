#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <bits/stdc++.h>

#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "linalg.h"
#include "ray.h"
#include "camera.h"
#include "color.h"
#include "light.h"
#include "object.h"
#include "source.h"
#include "sphere.h"
#include "plane.h"

struct RGBType
{
	double r;
	double g; 
	double b;
};

void saveBMP (const char *fileName, int w, int h, int dpi, RGBType *data)
{
	FILE *f;
	int totalPixels = w * h;
	int stride = 4 * totalPixels;
	int fileSize = 54 + stride;

	double factor = 39.375;
	int meter  = static_cast<int>(factor);

	int pixelPerMeter = dpi * meter;
	unsigned char bmpFileHeader[14] = {'B', 'M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpInfoHeader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};
	bmpFileHeader[ 2] = (unsigned char) (fileSize);
	bmpFileHeader[ 3] = (unsigned char) (fileSize>>8);
	bmpFileHeader[ 4] = (unsigned char) (fileSize>>16);
	bmpFileHeader[ 5] = (unsigned char) (fileSize>>24);

	bmpInfoHeader[ 4] = (unsigned char) (w);
	bmpInfoHeader[ 5] = (unsigned char) (w>>8);
	bmpInfoHeader[ 6] = (unsigned char) (w>>16);
	bmpInfoHeader[ 7] = (unsigned char) (w>>24);

	bmpInfoHeader[ 8] = (unsigned char) (h);
	bmpInfoHeader[ 9] = (unsigned char) (h>>8);
	bmpInfoHeader[ 10] = (unsigned char) (h>>16);
	bmpInfoHeader[ 11] = (unsigned char) (h>>24);

	bmpInfoHeader[ 21] = (unsigned char) (stride);
	bmpInfoHeader[ 22] = (unsigned char) (stride>>8);
	bmpInfoHeader[ 23] = (unsigned char) (stride>>16);
	bmpInfoHeader[ 24] = (unsigned char) (stride>>24);

	bmpInfoHeader[ 25] = (unsigned char) (pixelPerMeter);
	bmpInfoHeader[ 26] = (unsigned char) (pixelPerMeter>>8);
	bmpInfoHeader[ 27] = (unsigned char) (pixelPerMeter>>16);
	bmpInfoHeader[ 28] = (unsigned char) (pixelPerMeter>>24);
	
	bmpInfoHeader[ 29] = (unsigned char) (pixelPerMeter);
	bmpInfoHeader[ 30] = (unsigned char) (pixelPerMeter>>8);
	bmpInfoHeader[ 31] = (unsigned char) (pixelPerMeter>>16);
	bmpInfoHeader[ 32] = (unsigned char) (pixelPerMeter>>24);

	f = fopen(fileName, "wb");

	fwrite(bmpFileHeader, 1, 14, f);
	fwrite(bmpInfoHeader, 1, 40, f);

	for (int i = 0; i < totalPixels; i++)
	{
		RGBType rgb = data[i];

		double red = (data[i].r) * 255;
		double green = (data[i].g) * 255;
		double blue = (data[i].b) * 255;

		unsigned char color[3] = {(int)floor(blue), (int)floor(green), (int)floor(red)};

		fwrite(color, 1, 3, f);
	}
	fclose(f);
}

int winningObjectIndex(std::vector<double> object_intersections) {
    // return the index of the winning intersection
    int index_of_minimum_value;

    // prevent unnessary calculations
    if (object_intersections.size() == 0) {
        // if there are no intersections
        return -1;
    }
    else if (object_intersections.size() == 1) {
        if (object_intersections.at(0) > 0) {
            // if that intersection is greater than zero then its our index of minimum value
            return 0;
        }
        else {
            // otherwise the only intersection value is negative
            return -1;
        }
    }
    else {
        // otherwise there is more than one intersection
        // first find the maximum value

        double max = 0;
        for (int i = 0; i < object_intersections.size(); i++) {
            if (max < object_intersections.at(i)) {
                max = object_intersections.at(i);
            }
        }

        // then starting from the maximum value find the minimum positive value
        if (max > 0) {
            // we only want positive intersections
            for (int index = 0; index < object_intersections.size(); index++) {
                if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {
                    max = object_intersections.at(index);
                    index_of_minimum_value = index;
                }
            }

            return index_of_minimum_value;
        }
        else {
            // all the intersections were negative
            return -1;
        }
    }
}

Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction, std::vector<Object*> scene_objects, int index_of_winning_object, std::vector<Source*> light_sources, double accuracy, double ambientlight) {

    Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
    Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);

    if (winning_object_color.getColorSpecial() == 2) {
        // checkered/tile floor pattern

        int square = (int)floor(intersection_position.getVectX()) + (int)floor(intersection_position.getVectZ());

        if ((square % 2) == 0) {
            // black tile
            winning_object_color.setColorRed(0);
            winning_object_color.setColorGreen(0);
            winning_object_color.setColorBlue(0);
        }
        else {
            // white tile
            winning_object_color.setColorRed(1);
            winning_object_color.setColorGreen(1);
            winning_object_color.setColorRed(1);
        }
    }

    Color final_color = winning_object_color.colorScalar(ambientlight);

    if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
        // reflection from objects with specular intensity
        double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
        Vect scalar1 = winning_object_normal.vectMult(dot1);
        Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
        Vect scalar2 = add1.vectMult(2);
        Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
        Vect reflection_direction = add2.normalize();

        Ray reflection_ray (intersection_position, reflection_direction);

        // determine what the ray intersects with first
        std::vector<double> reflection_intersections;

        for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
            reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
        }

        int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

        if (index_of_winning_object_with_reflection != -1) {
            // reflection ray missed everthing else
            if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
                // determine the position and direction at the point of intersection with the reflection ray
                // the ray only affects the color if it reflected off something

                Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
                Vect reflection_intersection_ray_direction = reflection_direction;

                Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

                final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
            }
        }
    }

    for (int light_index = 0; light_index < light_sources.size(); light_index++) {
        Vect light_direction = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();

        float cosine_angle = winning_object_normal.dotProduct(light_direction);

        if (cosine_angle > 0) {
            // test for shadows
            bool shadowed = false;

            Vect distance_to_light = light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize();
            float distance_to_light_magnitude = distance_to_light.magnitude();

            Ray shadow_ray (intersection_position, light_sources.at(light_index)->getLightPosition().vectAdd(intersection_position.negative()).normalize());

            std::vector<double> secondary_intersections;

            for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
                secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
            }

            for (int c = 0; c < secondary_intersections.size(); c++) {
                if (secondary_intersections.at(c) > accuracy) {
                    if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
                        shadowed = true;
                    }
                }
                break;
            }

            if (shadowed == false) {
                final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->getLightColor()).colorScalar(cosine_angle));

                if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
                    // special [0-1]
                    double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
                    Vect scalar1 = winning_object_normal.vectMult(dot1);
                    Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
                    Vect scalar2 = add1.vectMult(2);
                    Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
                    Vect reflection_direction = add2.normalize();

                    double specular = reflection_direction.dotProduct(light_direction);
                    if (specular > 0) {
                        specular = pow(specular, 10);
                        final_color = final_color.colorAdd(light_sources.at(light_index)->getLightColor().colorScalar(specular*winning_object_color.getColorSpecial()));
                    }
                }

            }

        }
    }

    return final_color.clip();
}

template<typename T>
std::vector<double> linspace(T start_in, T end_in, int num_in)
{

    std::vector<double> linspaced;

    double start = static_cast<double>(start_in);
    double end = static_cast<double>(end_in);
    double num = static_cast<double>(num_in);

    if (num == 0) { return linspaced; }
    if (num == 1)
    {
        linspaced.push_back(start);
        return linspaced;
    }

    double delta = (end - start) / (num - 1);

    for(int i=0; i < num-1; ++i)
    {
        linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end); // I want to ensure that start and end
    // are exactly the same as the input
    return linspaced;
}

int main (int argc, char *argv[]) {
    std::cout << "rendering ..." << std::endl;
    std::vector<double> vec_1 = linspace(-10, 10, 100);
    clock_t t1, t2;
    t1 = clock();
    int count = 2;
    for (int vec_index = 0; vec_index < vec_1.size(); vec_index++) {
        int dpi = 72;
        int width = 640;
        int height = 480;
        int n = width * height;
        RGBType *pixels = new RGBType[n];
        int aadepth = 1;
        double aathreshold = 0.1;
        double aspectratio = (double) width / (double) height;
        double ambientlight = 0.2;
        double accuracy = 0.00000001;

        Vect O(0, 0, 0);
        Vect X(1, 0, 0);
        Vect Y(0, 1, 0);
        Vect Z(0, 0, 1);

        Vect new_sphere_location(1.75, -0.25, 0);

        Vect campos(3, 1.5, -4);

        Vect look_at(0, 0, 0);
        Vect diff_btw(campos.getVectX() - look_at.getVectX(), campos.getVectY() - look_at.getVectY(),
                      campos.getVectZ() - look_at.getVectZ());

        Vect camdir = diff_btw.negative().normalize();
        Vect camright = Y.crossProduct(camdir).normalize();
        Vect camdown = camright.crossProduct(camdir);
        Camera scene_cam(campos, camdir, camright, camdown);

        Color white_light(1.0, 1.0, 1.0, 0);
        Color pretty_green(0.5, 0.2, 1, 0.1);
        Color maroon(0.5, 0.25, 0.25, 0);
        Color tile_floor(1, 1, 1, 2);
        Color gray(0.5, 0.5, 0.5, 0);
        Color black(0.0, 0.0, 0.0, 0);
        Color bland_yellow(double(246.0/255.0), double(230.0/255.0), double(12.0/255.0), 0);

        Vect light_position(vec_1[vec_index], 10, -10);
        Light scene_light(light_position, white_light);
        std::vector < Source * > light_sources;
        light_sources.push_back(dynamic_cast<Source *>(&scene_light));


        // scene objects
        Sphere scene_sphere(O, 1.0, pretty_green);
        Plane scene_plane(Y, -1, bland_yellow);

        std::vector < Object * > scene_objects;
        scene_objects.push_back(dynamic_cast<Object *>(&scene_sphere));
        scene_objects.push_back(dynamic_cast<Object *>(&scene_plane));
        int thisOne;
        double xamnt, yamnt;

        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                thisOne = y * width + x;

                // start with no anti-aliasing
                if (width > height) {
                    xamnt = ((x + 0.5) / width) * aspectratio - (((width - height) / (double) height) / 2);
                    yamnt = ((height - y) + 0.5) / height;
                } else if (height > width) {
                    xamnt = (x + 0.5) / width;
                    yamnt = (((height - y) + 0.5) / height) / aspectratio - (((height - width) / (double) width) / 2);
                } else {
                    xamnt = (x + 0.5) / width;
                    yamnt = ((height - y) + 0.5) / height;
                }

                Vect cam_ray_origin = scene_cam.getCameraPosition();
                Vect cam_ray_direction = camdir.vectAdd(
                        camright.vectMult(xamnt - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();

                Ray cam_ray(cam_ray_origin, cam_ray_direction);
                std::vector<double> intersections;

                for (int index = 0; index < scene_objects.size(); index++) {
                    intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
                }

                int index_of_winning_object = winningObjectIndex(intersections);
                if (index_of_winning_object == -1) {
                    pixels[thisOne].r = 0;
                    pixels[thisOne].g = 0;
                    pixels[thisOne].b = 0;
                } else {
                    if (intersections.at(index_of_winning_object) > accuracy) {
                        Vect intersection_position = cam_ray_origin.vectAdd(
                                cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
                        Vect intersecting_ray_direction = cam_ray_direction;

                        Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction,
                                                              scene_objects, index_of_winning_object, light_sources,
                                                              accuracy, ambientlight);
                        pixels[thisOne].r = intersection_color.getColorRed();
                        pixels[thisOne].g = intersection_color.getColorGreen();
                        pixels[thisOne].b = intersection_color.getColorBlue();

                    }
                }
                //return a color
            }
        }
        //char*filename[10] = {'s','c','e','n','e','0','.','b','m','p'};
        //*fileName[  ] = new char
        std::string filenaam = "scene" + std::to_string(count) + ".bmp";
        int size_filenaam = filenaam.size();
        char char_filenaam[size_filenaam+1];
        std::strcpy(char_filenaam, filenaam.c_str());
        saveBMP(char_filenaam, width, height, dpi, pixels);
        count = count + 1;
    }
    return 0;
}