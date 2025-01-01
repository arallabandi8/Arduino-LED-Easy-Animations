#include <iostream>
#include "../lib/PNG.h"

int main(){
    PNG test("test.png");
    std::cout << "Image has dimensions " << test.getWidth() << " by " << test.getHeight() << "." << std::endl;
    test.save("result.png");

    PNG copy = test;
    std::cout << "Resizing to 500x200..." << std::endl;
    copy.resize(500, 200);
    copy.save("resize.png");

    PNG scaled = test;
    std::cout << "Scaling to 1000x1000" << std::endl;
    scaled.scale(1000,1000);
    scaled.save("scale.png");

    PNG bin = test;
    std::cout << "Binarifying with black and white..." << std::endl;
    bin.binarify(Pixel(255,255,255,255), Pixel(0,0,0,255));
    bin.save("binarify.png");

    PNG bin2 = test;
    std::cout << "Binarifying with dark blue and purple..." << std::endl;
    bin2.binarify(Pixel(0,0,179,255), Pixel(93,0,179,255));
    bin2.save("binarify2.png");

    PNG ard = test;
    std::cout << "Arduino-fying the image..." << std::endl;
    ard.scale(12,8);
    ard.binarify(Pixel(255,255,255,255), Pixel(0,0,0,255));
    ard.save("arduino.png");
}