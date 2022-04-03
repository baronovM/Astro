#pragma once
#include <nlopt.hpp>
#include "Astro.h"


class NloptImage {
public:
    SmartImage* img;
    Color test_color;
    string imagePath;
    NloptImage(string filePath, const Color& c) : test_color(c) {
        img = new SmartImage(filePath);
    }
    NloptImage(SmartImage* image, const Color& c) : img(image), test_color(c) {}
    ~NloptImage() {
        delete img;
    }
};



double myvfunc(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data);
double cntConstraint(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data);
double lowerR(const std::vector<double>& x, std::vector<double>& grad, void* data);
double upperR(const std::vector<double>& x, std::vector<double>& grad, void* data);
double signConstraint(const std::vector<double>& x, std::vector<double>& grad, void* data);