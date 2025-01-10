#include "arduino-animation.h"
#include <fstream>
#include <iostream>

/*@@@@@@@@@@@@@@@@@@@@@@@
Basic class functionality
@@@@@@@@@@@@@@@@@@@@@@@@@*/

Animation::Animation(size_t f) : fps_(f), sameDims_(true) {
    if (f <= 0){
        throw std::runtime_error("Animation constructor ERROR: FPS cannot be less than or equal to 0.");
    }
}

Animation::Animation(size_t f, std::vector<PNG> famey, bool sd) : fps_(f), sameDims_(sd), frames_(famey) {
    if (f <= 0){
        throw std::runtime_error("Animation constructor ERROR: FPS cannot be less than or equal to 0.");
    }
    if (sameDims_){
        width_ = famey[0].getWidth();
        height_ = famey[0].getHeight();
    }
    else{
        width_ = 0, height_ = 0;
    }
}

size_t Animation::getFPS(){return fps_;}

void Animation::setFPS(size_t newFPS){
    if (newFPS <= 0){
        throw std::runtime_error("Animation::setFPS() ERROR: FPS cannot be less than or equal to 0.");
    }
    fps_ = newFPS;
}

std::vector<PNG> Animation::getFrames(){return frames_;}
std::vector<PNG>& Animation::getFramesRef(){return frames_;}

float Animation::getSize(){
    // 96 bits per frame, 8 bits in a byte
    return (96*frames_.size()) / 8.000;
}

/*@@@@@@@@@@@@@@
Frame operations
@@@@@@@@@@@@@@@@*/

void Animation::addFrame(PNG myFrame){
    if (!sameDims_){
        throw std::runtime_error("Animation::addFrame() ERROR: Cannot call addFrame() on animations with variable dimensions.");
    }

    /* If animation is empty, add frame and set the dimensions accordingly. If not, modify the frame to
    match the dimensions of the other frames. */
    if (frames_.empty()){
        frames_.emplace_back(myFrame);
        width_ = myFrame.getWidth();
        height_ = myFrame.getHeight();
    }
    else{
        myFrame.scale(width_, height_);
        frames_.emplace_back(myFrame);
    }
}

void Animation::addFrameArd(PNG myFrame){
    if (frames_.empty()){ // set dimensions for empty animation
        width_ = myFrame.getWidth();
        height_ = myFrame.getHeight();
    }
    myFrame.scale(12, 8);
    myFrame.binarify(BLACK, WHITE);
    frames_.emplace_back(myFrame);

    // check if dimensions match and warn if they do not
    if ((width_ != 12 || height_ != 8) && sameDims_){
        sameDims_ = false;
        width_ = 0;
        height_ = 0;
        std::cout << "Animation::addFrameArd() WARNING: Frame added with dimensions " << std::to_string(myFrame.getWidth()) << "x" <<
        std::to_string(myFrame.getHeight()) << " does not match animation dimensions " << std::to_string(width_) << "x" << std::to_string(height_)
        << ".";
    }
}

void Animation::addFrameUnchanged(PNG myFrame){
    frames_.emplace_back(myFrame);
    if (frames_.empty()){
        width_ = myFrame.getWidth();
        height_ = myFrame.getHeight();
    }

    if (sameDims_ && (myFrame.getWidth() != width_ || myFrame.getHeight() != height_)){
        sameDims_ = false;
        width_ = 0;
        height_ = 0;
        std::cout << "Animation::addFrameUnchanged() WARNING: Frame added with dimensions " << std::to_string(myFrame.getWidth()) << "x" <<
        std::to_string(myFrame.getHeight()) << " does not match animation dimensions " << std::to_string(width_) << "x" << std::to_string(height_)
        << ".";
    }
}

std::vector<u_int32_t> Animation::frameToArduino(PNG myFrame, Pixel domColorA, Pixel domColorB){
    /* For safety, make sure the frame is in correct Arduino format. */
    myFrame.resize(12,8);
    myFrame.binarify(domColorA, domColorB);

    /* Each frame can be represented as 3 32-bit sequences. */
    std::vector<u_int32_t> result;
    result.resize(3);

    /* For every pixel in myFrame, check if it matches either domColorA or domColorB. If
    it matches A, write a "1". Otherwise, write a "0".*/
    for (size_t y=0; y < 8; y++){
        for (size_t x=0; x < 12; x++){
            int overallIdx = x + y*12;
            u_int32_t bitmask;
            u_int32_t& num = result[overallIdx/32]; // fetch the correct 32-bit integer to modify based on index
            Pixel curr = myFrame.getPixel(x,y);
            if (curr == domColorA){
                /* Push a 1 to the correct place. */
                bitmask = 0x1 << overallIdx%32; // put 1 in correct bit place
                num = num | bitmask; // OR with the bitmask to maintain current 1s while adding the new 1
            }
            else{
                /* Push a 0 to the correct place. */
                bitmask = ~(0x1 << overallIdx%32); // put 1 in correct bit place, then NOT the whole number to turn it into a 0
                num = num & bitmask; // AND with bitmask to maintain current 0s while adding the new 0
            }
        }
    }

    return result;
}

/*@@@@@@@@@@@@@@@@@@@@@@@
Animation-wide operations
@@@@@@@@@@@@@@@@@@@@@@@@@*/

void Animation::scale(unsigned x, unsigned y){
    /* Check that dimensions are valid before proceeding. */
    if (x < 1 || y < 1){
        throw std::runtime_error("Animation::scale() ERROR: New dimensions must be greater than 0.");
    }

    /* Go through each animation in the frames_ vector and scale them to the request dimensions. */
    for (PNG& f : frames_){
        f.scale(x,y);
    }

    /* Update member variables.*/
    sameDims_ = true;
    width_ = x;
    height_ = y;
}

void Animation::arduinofy(){
    scale(12,8);
    for (PNG& f : frames_){
        f.binarify(BLACK, WHITE);
    }
}

std::vector<std::vector<u_int32_t>> Animation::animationToArduino(){
    std::vector<std::vector<u_int32_t>> sequence;
    for (PNG f : getFrames()){
        sequence.emplace_back(frameToArduino(f, BLACK, WHITE));
    }

    return sequence;
}