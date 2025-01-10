#include "../lib/PNG.h"

#define BLACK Pixel(0,0,0,255)
#define WHITE Pixel(255,255,255,255)

class Animation{
    public:
        /**
         * Default constructor. Creates an empty animation.
         */
        Animation(size_t = 15);

        /**
         * Constructor with vector of frames. Creates animation
         * based on vector of frames.
         */
        Animation(size_t, std::vector<PNG>, bool);

        /**
         * Getter for fps.
         * @return Current FPS of the animation.
         */
        size_t getFPS();

        /**
         * Setter for fps.
         * @param newFPS The new FPS to use for the animation. Must be greater than 0.
         */
        void setFPS(size_t newFPS);

        /**
         * Getter for frames. This will create a copy.
         * @return A vector containing the frames of the animation.
         */
        std::vector<PNG> getFrames();

        /**
         * Getter for frames. Since this is a reference, this
         * will allow for direct modification.
         * @return A reference to the frames_ vector of the Animation object.
         */
        std::vector<PNG>& getFramesRef();

        /**
         * Compute the current size of the animation based on how many frames are in
         * the frame vector. For Arduino UNO R4 LED Matrix, each frame in the most
         * memory-efficient implementation takes 96 bits, where each bit represents
         * the state of an LED.
         * @return The estimated size of the animation once translated to Arduino code.
         */
        float getSize();

        /**
         * Adds a frame to the end of the animation. Method will take into account the
         * width_ and height_ of other frames and modify new frame to match dimensions.
         * This is meant for ordinary animations that will not go on the Arduino UNO R4
         * LED Matrix.
         * addFrame() CANNOT BE CALLED ON ANIMATIONS THAT ALREADY HAVE VARIABLE DIMS.
         * A runtime error will be triggered to avoid complications.
         * @param myFrame The frame to be added.
         */
        void addFrame(PNG myFrame);

        /**
         * Adds a frame to the end of the animation. Method accounts for the source image
         * to not be in the correct format, and as such will scale it down to 12x8 and
         * binarify it with black and white colors.
         * @param myFrame The frame to be added.
         */
        void addFrameArd(PNG myFrame);

        /**
         * Adds a frame to the end of the animation. Does not perform scaling or
         * binarifying, so original image stays intact. This is meant for regular
         * animations that will not go on the Arduino UNO R4 LED Matrix. It is also
         * mainly for testing; dimensions are not matched.
         * @param myFrame The frame to be added.
         */
        void addFrameUnchanged(PNG myFrame);

        /**
         * Scales all frames in the animation to the requested dimensions. Function
         * will always ensure uniform dimensions for all frames, and as such, sameDims_
         * will be set to true.
         * @param x The new width
         * @param y The new height
         */
        void scale(unsigned x, unsigned y);

        /**
         * Modifies all frames in the animation to be compatible with the Arduino
         * UNO R4 LED Matrix by scaling down to 12x8 and binarifying with black
         * and white colors. Operation is not reversible.
         */
        void arduinofy();

        /**
         * Converts a frame to three 32-bit integers that store the states of the
         * 96 LEDs as 1 or 0. Frame must be in LED format (12x8, binarified)
         * before calling this function.
         * @param myFrame The frame to be converted
         * @param domColorA The first color of the image. This will be the LED ON color.
         * @param domColorB The second color of the image. This will be the LED OFF color.
         */
        std::vector<u_int32_t> frameToArduino(PNG myFrame, Pixel domColorA, Pixel domColorB);

        /**
         * Converts an entire Animation to the Arduino format of three 32-bit integers
         * to store the states of the 96 LEDs per frame. Basically, it repeatedly
         * calls frameToArduino.
         * Function will automatically call arduinofy().
         * Function cannot be called on an empty animation. (no shit...)
         * @return A vector of vectors, where each vector is the state of the 96 LEDs
         * for that frame.
         */
        std::vector<std::vector<u_int32_t>> Animation::animationToArduino();

    private:
        size_t fps_; // Frames per second of the animation. The lower, the longer.
        unsigned width_;
        unsigned height_;
        bool sameDims_; // Stores whether or not all frames are of the same dimensions.
        std::vector<PNG> frames_; // Vector of images that are part of the animation.
};