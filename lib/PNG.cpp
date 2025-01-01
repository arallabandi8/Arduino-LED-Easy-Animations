/******************************************************************************
 * Project:    Arduino LED Easy Animations
 * File:       PNG.cpp
 * Author:     Adarsh Rallabandi
 * Created:    2024-12-30
 * Updated:    2024-12-31
 *
 * Description:
 *   This file implements the PNG class and Pixel struct specified in PNG.h,
 *   providing necessary functionality with image manipulation required for
 *   other parts of the project.
 *
 * License:
 *   Licensed under GNU GPL. See LICENSE file for details.
 *
 ******************************************************************************/

#include "PNG.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <stdexcept>

/*@@@@@@@@@@@@@
Pixel functions
@@@@@@@@@@@@@@@*/

/**
 * Helper function for PNG::binarify() as well as the comparison operators
 * for the Pixel struct.
 * Computes the Euclidian distance between two Pixels in RGBA space.
 * @param pointA, pointB The two points to compare. Stored as vectors of unsigned
 * ints only becuase Pixel is a private struct of PNG class.
 * @return The Euclidian distance
 */
float euclidianDistance(std::vector<unsigned> pointA, std::vector<unsigned> pointB){
    return sqrt(pow(pointA[0] - pointB[0], 2) + pow(pointA[1] - pointB[1], 2) + pow(pointA[2] - pointB[2], 2) + pow(pointA[3] - pointB[3], 2));
}

/*@@@@@@@@@@@@@@@@@@@@@@@
Basic class functionality
@@@@@@@@@@@@@@@@@@@@@@@@@*/

PNG::PNG(unsigned w, unsigned h){
    width_ = w;
    height_ = h;

    std::string currError = "PNG constructor ERROR: Memory allocation failed.";

    /* Allocate necessary memory for pixels_ array. */
    pixels_.resize(width_*height_);
}

PNG::PNG(std::string filepath){
    readFromFile(filepath);
}

PNG::PNG(PNG& other){
    width_ = other.width_;
    height_ = other.height_;
    pixels_ = other.pixels_;
}

unsigned PNG::getWidth(){
    return width_;
}

unsigned PNG::getHeight(){
    return height_;
}

Pixel& PNG::getPixel(unsigned x, unsigned y){
    if (x > width_ || y > height_){
        throw std::runtime_error("PNG::getPixel() ERROR: Specified coordinates (" + std::to_string(x) + ", " + std::to_string(y) +
        ") are out of bounds. Image dimensions are (" + std::to_string(width_) + ", " + std::to_string(height_) + ").");
    }

    return pixels_[x + y*width_];
}

/*@@@@@@@@@@@@@@@@@@@@@@@
File I/O related functions
@@@@@@@@@@@@@@@@@@@@@@@@@*/

void PNG::save(std::string filepath){
    writeToFile(filepath);
}

void PNG::readFromFile(std::string filepath){
    FILE* f = fopen(filepath.c_str(), "rb");
    if (!f){
        throw std::runtime_error("PNG::readFromFile() ERROR: Failed to open file for reading. Does the file exist?");
    }
    
    /* Create structs for reading the information from our PNG. */
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);
    if (!png || !info){
        fclose(f);
        throw std::runtime_error("PNG::readFromFile() ERROR: Failed to create PNG read struct.");
    }
    if (setjmp(png_jmpbuf(png))){
        fclose(f);
        throw std::runtime_error("PNG::readFromFile() ERROR: setjmp did not return 0.");
    }
    
    /* Set our PNG struct's source file to f and read the info from f into our info struct. */
    png_init_io(png, f);
    png_read_info(png, info);

    /* Get dimensions from image and allocate necessary memory for pixels_ vector. */
    width_ = png_get_image_width(png, info);
    height_ = png_get_image_height(png, info);
    std::cout << "PNG::readFromFile(): Image dimensions are (" << width_ << ", " << height_ << ")." << std::endl;
    pixels_.resize(width_*height_);

    /* Ensure that the image is 8-bit depth and of RGB or RGBA color type. */
    auto colorType = png_get_color_type(png, info);
    auto bitDepth = png_get_bit_depth(png, info);
    if (bitDepth == 16){
        std::cout << "PNG was 16-bit depth. Stripping the second byte..." << std::endl;
        png_set_strip_16(png);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE){
        std::cout << "PNG had palette color type. Setting to RGB..." << std::endl;
        png_set_palette_to_rgb(png);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8){
        std::cout << "PNG had gray color type and bit depth less than 8. Correcting..." << std::endl;
        png_set_expand_gray_1_2_4_to_8(png);
    }
    if (png_get_valid(png, info, PNG_INFO_tRNS)){
        png_set_tRNS_to_alpha(png);
    }
    if (colorType == PNG_COLOR_TYPE_RGB || colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_PALETTE){
        std::cout << "PNG did not have alpha channel. Filling with 0xff..." << std::endl;
        png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
    }
    if (colorType == PNG_COLOR_TYPE_GRAY || colorType == PNG_COLOR_TYPE_GRAY_ALPHA){
        std::cout << "PNG had gray color type. Setting to RGB..." << std::endl;
        png_set_gray_to_rgb(png);
    }

    png_read_update_info(png, info); // update the info after we change bit depth and color type

    /* Read the PNG image into an array of row pointers. Then, we can use those row pointers
    to get the RGBA information into the pixels_ array.*/
    png_bytep* imageByRows = new png_bytep[height_];
    for (int j=0; j < height_; j++){
        // each png_byte stores r, g, b, or a information. As such, we allocate 4 for each pixel.
        imageByRows[j] = (png_byte*) malloc(png_get_rowbytes(png,info));
    }
    png_read_image(png, imageByRows);

    // Copy over data from imageByRows to pixels_
    for (int j = 0; j < height_; j++){
        for (int i = 0; i < width_; i++){
            png_bytep currPx = &(imageByRows[j][i*4]); // create pointer to current 4 png_bytes
            pixels_[i + j*width_] = Pixel(currPx[0], currPx[1], currPx[2], currPx[3]);
        }
        delete[] imageByRows[j]; // save memory and delete the current row 
    }
    delete[] imageByRows; // delete entire imageByRows now that we are done with it

    png_destroy_read_struct(&png, &info, nullptr);
    fclose(f);
}

void PNG::writeToFile(std::string filepath){
    FILE* f = fopen(filepath.c_str(), "wb");
    if (!f){
        throw std::runtime_error("PNG::writeToFile() ERROR: Could not open or create file for writing. Do we have write permissions?");
    }

    /* Create structs to write the information to our png. */
    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    png_infop info = png_create_info_struct(png);

    if (!png || !info){
        fclose(f);
        throw std::runtime_error("PNG::writeToFile() ERROR: Failed to create PNG write struct.");
    }

    /* Prepare to write to our png by setting the correct information to and from f. Specifies the
    8-bit depth, RGBA color type, and defaults for the other options. */
    png_init_io(png, f);
    png_set_IHDR(png, info, width_, height_, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, info);

    /* Copy the PNG information into an array of row pointers. Then, we can use those row pointers
    to put the image information into our png struct. */
    png_bytep* imageByRows = new png_bytep[height_];
    for (unsigned j=0; j < height_; j++){
        imageByRows[j] = new png_byte[4*width_];
        for (unsigned i=0; i < width_; i++){ // in the same breath, we may write pixels!!!!
            Pixel& source = getPixel(i,j);
            png_bytep currPx = &(imageByRows[j][i*4]);
            /* Copy RGBA values to the png byte*/
            currPx[0] = source.red, currPx[1] = source.green, currPx[2] = source.blue, currPx[3] = source.alpha;
        }
    }

    // save changes to the png struct
    png_write_image(png, imageByRows);
    png_write_end(png, nullptr);

    /* Free up any memory still being used and close file. */ 
    for (int j=0; j < height_; j++){
        delete[] imageByRows[j];
    }
    delete[] imageByRows;

    png_destroy_write_struct(&png, &info);
    fclose(f);
}

/*@@@@@@@@@@@@@@@@
Image manipulation
@@@@@@@@@@@@@@@@@@*/

void PNG::resize(unsigned newX, unsigned newY){
    /* Handle 0 case. */
    if (newX == 0 || newY == 0){
        throw std::runtime_error("PNG::scale() ERROR: New dimensions must be greater than 0. Provided dimensions were (" + std::to_string(newX)
        + ", " + std::to_string(newY) + ").");
    }

    /* Just call calloc to allocate more memory and reinitialize pixels. */
    std::vector<Pixel> newPixels;
    newPixels.resize(newX*newY);

    if (newX < width_ || newY < height_){
        std::cout << "PNG is being resized to smaller dimensions. Some image data will be lost." << std::endl;

        for (unsigned i=0; i < newX; i++){
            for (unsigned j=0; j < newY; j++){
                newPixels[i + j*newX] = getPixel(i,j);
            }
        }
    } else{
        for (unsigned i=0; i < width_; i++){
            for (unsigned j=0; j < height_; j++){
                newPixels[i + j*newX] = getPixel(i,j);
            }
        }
    }

    /* Update width_ and height_ */
    width_ = newX;
    height_ = newY;
}

void PNG::scale(unsigned newX, unsigned newY){
    /* Handle 0 case. */
    if (newX == 0 || newY == 0){
        throw std::runtime_error("PNG::scale() ERROR: New dimensions must be greater than 0. Provided dimensions were (" + std::to_string(newX)
        + ", " + std::to_string(newY) + ").");
    }

    /* Create new pixel array with new dimensions. */
    std::vector<Pixel> newPixels;
    newPixels.resize(newX*newY);

    /* Calculate the scale factors to ensure that new image scales correctly. */
    float scaleX = (float) newX / (float) width_;
    float scaleY = (float) newY / (float) height_;

    /* Map new pixels to old ones using the scale factors calculated above. */
    for (unsigned x=0; x < newX; x++){
        for (unsigned y=0; y < newY; y++){
            unsigned sourceX = (unsigned) (x/scaleX);
            unsigned sourceY = (unsigned) (y/scaleY);
            // Safety check to ensure sourceX and sourceY are in bounds
            sourceX = std::min(sourceX, width_ - 1);
            sourceY = std::min(sourceY, height_ - 1);

            newPixels[x + y*newX] = getPixel(sourceX, sourceY);
        }
    }

    /* Update member variables. */
    pixels_ = newPixels;
    width_ = newX;
    height_ = newY;
}

/**@todo
 * Sometimes creating weird artifacts, especially on
 * bad apple images. This causes distortion at lower resolutions.
 */
void PNG::binarify(Pixel colorA, Pixel colorB){
    /* Create vector representations of Pixels. */
    std::vector<unsigned> vecA = {colorA.red, colorA.green, colorA.blue, colorA.alpha};
    std::vector<unsigned> vecB = {colorB.red, colorB.green, colorB.blue, colorB.alpha};

    /* Go through each point in the image, compute distances to A and B, then change its
    color to the one it is closer to. */
    for (int x=0; x < width_; x++){
        for (int y=0; y < height_; y++){
            Pixel& currPt = getPixel(x,y);
            std::vector<unsigned> curr = {currPt.red, currPt.green, currPt.blue, currPt.alpha}; // vector conversion

            // Compute distances
            float distA = euclidianDistance(curr, vecA);
            float distB = euclidianDistance(curr, vecB);

            if (distA > distB){
                currPt = colorB;
            } else{ // in case of tie, A wins
                currPt = colorA;
            }
        }
    }
}