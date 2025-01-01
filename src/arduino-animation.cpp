#include "arduino-animation.h"
#include <fstream>

/*@@@@@@@@@@@@@@@@@@@@@@@
Basic class functionality
@@@@@@@@@@@@@@@@@@@@@@@@@*/

Animation::Animation(size_t f) : fps_(f) {
    if (f <= 0){
        throw std::runtime_error("Animation constructor ERROR: FPS cannot be less than or equal to 0.");
    }
}

Animation::Animation(size_t f, std::vector<PNG> famey) : fps_(f), frames_(famey) {
    if (f <= 0){
        throw std::runtime_error("Animation constructor ERROR: FPS cannot be less than or equal to 0.");
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
    myFrame.scale(12, 8);
    myFrame.binarify(BLACK, WHITE);
    frames_.emplace_back(myFrame);
}

void Animation::addFrameNormal(PNG myFrame){
    frames_.emplace_back(myFrame);
}