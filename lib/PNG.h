/******************************************************************************
 * Project:    Arduino LED Easy Animations
 * File:       PNG.h
 * Author:     Adarsh Rallabandi
 * Created:    2024-12-30
 * Updated:    2024-12-31
 *
 * Description:
 *   This file defines the PNG class and Pixel struct, which provides functionality
 *   of PNG images through libpng. Pixels are stored in RGBA format. Purpose of
 *   class is to allow for easy manipulation of images.
 *
 * License:
 *   Licensed under GNU GPL. See LICENSE file for details.
 *
 ******************************************************************************/

#include <png.h>
#include <string>
#include <vector>

/* Simple Pixel struct to hold RGBA information of a pixel.*/
struct Pixel{
    unsigned red;
    unsigned green;
    unsigned blue;
    unsigned alpha;

    /**
     * Default Pixel constructor. Values default to 0 if not provided,
     * except for alpha which will always be opaque unless specified.
     * @param r,g,b The red, green, and blue values respectively.
     * @param a The alpha value. Determines transparency of the image.
     */
    Pixel(unsigned r = 0, unsigned g = 0, unsigned b = 0, unsigned a = 255) : red(r), green(g), blue(b), alpha(a) {};
};

class PNG{
    public:
        /**
         * Parametrized constructor. Sets width and height variables correctly and
         * allocates necessary memory for pixels_ vector.
         * @param w The desired width
         * @param h The desired height
         * @return An empty PNG object
         */
        PNG(unsigned w = 0, unsigned h = 0);

        /**
         * File constructor. Creates a PNG object given a file path using the supreme
         * capabilities of libpng.
         * @param filepath A string with the exact or relative file path to a real .png file
         * @return A PNG object representing the provided .png file.
         */
        PNG(std::string filepath);

        /**
         * Copy constructor. Creates a copy of another PNG object.
         * @param other The other PNG object
         * @return A copy of other
         */
        PNG(PNG& other);

        /**
         * Width access operator.
         * @return The width of the image.
         */
        unsigned getWidth();

        /**
         * Height access operator.
         * @return The height of the image.
         */
        unsigned getHeight();

        /**
         * Pixel access operator. Since a reference is returned, direct modification of
         * a Pixel is supported.
         * @param x,y The coordinates of the desired pixel. Must be in-bounds.
         * @return A reference to the pixel.
         */
        Pixel& getPixel(unsigned x, unsigned y);

        /**
         * Resize the canvas of the image. Note that this DOES NOT SCALE THE IMAGE! The
         * canvas is just enlargened with the original image remaining in the top-left
         * corner.
         * To put it simply, all this does is resize the vector.
         * @param newX, newY The new dimensions of the image. Must be greater than 0.
         */
        void resize(unsigned newX, unsigned newY);

        /**
         * Resizes and scales the image. This does not preserve the aspect ratio of the
         * original image and may stretch or squeeze it.
         * @param newX, newY The new dimensions of the image. Must be greater than 0.
         */
        void scale(unsigned newX, unsigned newY);

        /**
         * Changes all pixels into one of two colors. This requires comparisons to
         * determine the degree of similarity between two Pixels.
         * @param colorA, colorB The two colors you wish to use for the binarify operation
         */
        void binarify(Pixel colorA, Pixel colorB);

        /**
         * Creates a PNG file from the information of the object and stores image using
         * libpng writing capabilities.
         * @param filepath A string with the exact or relative file path to a real .png file
         */
        void save(std::string filepath);

    private:
        /* ================
           Member variables
           ================ */
        unsigned width_;
        unsigned height_;
        std::vector<Pixel> pixels_; // row-major order

        /* =================
           Private functions
           ================= */

        /**
         * Private helper function for file constructor. Uses libpng to read a PNG file
         * and set member values accordingly.
         * @param filepath A string with the exact or relative file path to a real .png file
         */
        void readFromFile(std::string filepath);

        /**
         * Private helper function for "save." Uses libpng to write to a PNG file with
         * values from member variables.
         * @param filepath A string with the exact or relative file path to a real .png file
         */
        void writeToFile(std::string filepath);
};

