//
//  Blur.hpp
//  NekoUI
//
//  Created by Никита Исаенко on 31.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

////////////////////////////////////
///
/// Algorithm by Ivan Kuckir (http://blog.ivank.net/fastest-gaussian-blur.html)
/// Implementation by https://github.com/WurmD/LowPass
///
////////////////////////////////////

#ifndef Blur_hpp
#define Blur_hpp

#include <cmath>
#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>

#include "../../Engine/EntitySystem.hpp"
#include "../../Engine/GUI/Blur.hpp"

using namespace ns;
using namespace std;

namespace ns
{
    namespace GUI
    {
        vector<int> boxesForGauss(float sigma, int n);
        void boxBlurH_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r);
        void boxBlurT_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r);
        void boxBlur_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r);
        void gaussBlur_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r);
        void LowPass(vector<unsigned char>* target, vector<unsigned char>* source, int w, int h, int radius);
        sf::Image Blur(const sf::Image& image, int power = 10, int skip = 5);
    }
}

#endif /* Blur_hpp */
