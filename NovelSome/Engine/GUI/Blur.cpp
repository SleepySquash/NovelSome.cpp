//
//  Blur.cpp
//  NekoUI
//
//  Created by Никита Исаенко on 31.01.2020.
//  Copyright © 2020 Melancholy Hill. All rights reserved.
//

#include "Blur.hpp"

namespace ns
{
    namespace GUI
    {
        vector<int> boxesForGauss(float sigma, int n)  // standard deviation, number of boxes
        {
            auto wIdeal = sqrt((12 * sigma*sigma / n) + 1);  // Ideal averaging filter width
            int wl = floor(wIdeal);
            if (wl % 2 == 0)
                wl--;
            int wu = wl + 2;

            auto mIdeal = (12 * sigma*sigma - n * wl*wl - 4 * n*wl - 3 * n) / (-4 * wl - 4);
            int m = round(mIdeal);
            // var sigmaActual = Math.sqrt( (m*wl*wl + (n-m)*wu*wu - n)/12 );

            vector<int> sizes(n);
            for (auto i = 0; i < n; i++)
                sizes[i] = i < m ? wl : wu;
            return sizes;
        }

        void boxBlurH_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r)
        {
            float iarr = 1.f / (r + r + 1);
            for (auto i = 0; i < h; i++) {
                auto ti = i * w, li = ti, ri = ti + r;
                auto fv = scl[ti], lv = scl[ti + w - 1];
                auto val = (r + 1)*fv;
                for (auto j = 0; j < r; j++) val += scl[ti + j];
                for (auto j = 0; j <= r; j++) { val += scl[ri++] - fv;   tcl[ti++] = round(val*iarr); }
                for (auto j = r + 1; j < w - r; j++) { val += scl[ri++] - scl[li++];   tcl[ti++] = round(val*iarr); }
                for (auto j = w - r; j < w; j++) { val += lv - scl[li++];   tcl[ti++] = round(val*iarr); }
            }
        }
        void boxBlurT_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r)
        {
            float iarr = 1.f / (r + r + 1);
            for (auto i = 0; i < w; i++) {
                auto ti = i, li = ti, ri = ti + r * w;
                auto fv = scl[ti], lv = scl[ti + w * (h - 1)];
                auto val = (r + 1)*fv;
                for (auto j = 0; j < r; j++) val += scl[ti + j * w];
                for (auto j = 0; j <= r; j++) { val += scl[ri] - fv;  tcl[ti] = round(val*iarr);  ri += w; ti += w; }
                for (auto j = r + 1; j < h - r; j++) { val += scl[ri] - scl[li];  tcl[ti] = round(val*iarr);  li += w; ri += w; ti += w; }
                for (auto j = h - r; j < h; j++) { val += lv - scl[li];  tcl[ti] = round(val*iarr);  li += w; ti += w; }
            }
        }
        void boxBlur_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r)
        {
            for (auto i = 0; i < scl.size(); i++) tcl[i] = scl[i];
            boxBlurH_4(tcl, scl, w, h, r);
            boxBlurT_4(scl, tcl, w, h, r);
        }
        void gaussBlur_4(vector<unsigned char>& scl, vector<unsigned char>& tcl, int w, int h, int r)
        {
            auto bxs = boxesForGauss(r, 3);
            boxBlur_4(scl, tcl, w, h, (bxs[0] - 1) / 2);
            boxBlur_4(tcl, scl, w, h, (bxs[1] - 1) / 2);
            boxBlur_4(scl, tcl, w, h, (bxs[2] - 1) / 2);
        }

        void LowPass(vector<unsigned char>* target, vector<unsigned char>* source, int w, int h, int radius)
        {
            //vector<unsigned char> lowpass = *source; // copy constructor
            gaussBlur_4(*source, *target, w, h, radius);
        }

        sf::Image Blur(const sf::Image& image, int power, int skip)
        {
            int w = image.getSize().x/skip, h = image.getSize().y/skip;
            
            vector<unsigned char> vR(w*h, 0),
                                  vG(w*h, 0),
                                  vB(w*h, 0);
            for (int x = 0, xr = 0; xr < w; ++xr, x += skip)
                for (int y = 0, yr = 0; yr < h; ++yr, y += skip)
                {
                    vR[yr*w + xr] = image.getPixel(x, y).r;
                    vG[yr*w + xr] = image.getPixel(x, y).g;
                    vB[yr*w + xr] = image.getPixel(x, y).b;
                }
            
            vector<unsigned char> resR(vR.size(), 0),
                                  resG(vG.size(), 0),
                                  resB(vB.size(), 0);
            std::thread rChannel(LowPass, &resR, &vR, w, h, power),
                        gChannel(LowPass, &resG, &vG, w, h, power),
                        bChannel(LowPass, &resB, &vB, w, h, power);
            rChannel.join();
            gChannel.join();
            bChannel.join();
            
            sf::Image blur;
            blur.create(w, h);
            for (int x = 0; x < w; ++x)
                for (int y = 0; y < h; ++y)
                    blur.setPixel(x, y, { resR[y*w + x], resG[y*w + x], resB[y*w + x] });
            
            return blur;
        }
    }
}
