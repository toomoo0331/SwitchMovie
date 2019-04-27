//
// Created by kei666 on 18/10/27.
//
#include <iostream>

#include "switch_movie/SwitchMovie.h"

int main()
{
    float x = 1.0;
    int y = 3;
    sw::SwitchMovie sw(4, y);

    for (int i = 0; i < 2000; i++) {
        {
            std::vector<float> x;
            x.push_back(0.1);
            x.push_back(0.2);
            x.push_back(0.3);
            x.push_back(0.4);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.5);
            x.push_back(0.4);
            x.push_back(0.6);
            x.push_back(0.2);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.4);
            x.push_back(0.1);
            x.push_back(0.6);
            x.push_back(0.1);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.7);
            x.push_back(0.4);
            x.push_back(0.8);
            x.push_back(0.1);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.1);
            x.push_back(0.2);
            x.push_back(0.3);
            x.push_back(0.4);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.5);
            x.push_back(0.4);
            x.push_back(0.6);
            x.push_back(0.2);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.1);
            x.push_back(0.1);
            x.push_back(0.6);
            x.push_back(0.3);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.1);
            x.push_back(0.4);
            x.push_back(0.8);
            x.push_back(0.7);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.1);
            x.push_back(0.2);
            x.push_back(0.3);
            x.push_back(0.4);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.5);
            x.push_back(0.4);
            x.push_back(0.6);
            x.push_back(0.2);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.4);
            x.push_back(0.1);
            x.push_back(0.6);
            x.push_back(0.3);
            sw.add(x);
        }
        {
            std::vector<float> x;
            x.push_back(0.7);
            x.push_back(0.4);
            x.push_back(0.8);
            x.push_back(0.7);
            sw.add(x);
        }
    }
    std::cout << sw.numVertex() << std::endl;
    std::vector<int> a = sw.optimisation();
    for (auto aa : a)
        std::cout << aa << std::endl;

    return EXIT_SUCCESS;
}