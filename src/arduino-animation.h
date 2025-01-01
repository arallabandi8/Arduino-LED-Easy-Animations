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
        Animation(size_t, std::vector<PNG>);

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
         * Adds a frame to the end of the animation. Method accounts for the source image
         * to not be in the correct format, and as such will scale it down to 12x8 and
         * binarify it with black and white colors.
         * @param myFrame The frame to be added.
         */
        void addFrame(PNG myFrame);

        /**
         * Adds a frame to the end of the animation. Does not perform scaling or
         * binarifying, so original image stays intact. This is meant for regular
         * animations that will not go on the Arduino UNO R4 LED Matrix.
         * @param myFrame The frame to be added.
         */
        void addFrameNormal(PNG myFrame);

        /**
         * Modifies all frames in the animation to be compatible with the Arduino
         * UNO R4 LED Matrix by scaling down to 12x8 and binarifying with black
         * and white colors. Operation is not reversible.
         */
        void arduinofy();

    private:
        size_t fps_; // Frames per second of the animation. The lower, the longer.
        std::vector<PNG> frames_; // Vector of images that are part of the animation.
};