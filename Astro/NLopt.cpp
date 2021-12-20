#include <nlopt.hpp>
#include "Astro.h"

class NloptImage {
public:
    PlanImage* img;
    Color test_color;
    string imagePath;
    NloptImage(string filePath, const Color& c) : test_color(c) {
        img = new PlanImage(filePath);
    }
    NloptImage(PlanImage* image, const Color& c) : img(image), test_color(c) {}
    ~NloptImage() {
        delete img;
    }
};


double myvfunc(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(my_func_data);

    ostringstream temp;
    for (int i = 0; i < NUMCOEF; ++i)
        temp << "__" << x[i];
    PlanImage* img;
    //cout << "out/" + nloptimg->imagePath + temp.str() + ".png";
    if (!filesystem::exists("out/" + nloptimg->imagePath + temp.str() + ".png")) {
        img = distorce(nloptimg->img, x.data());
        img->saveToFile("out/" + nloptimg->imagePath + temp.str() + ".png");
    }
    else {
        img = new PlanImage("out/" + nloptimg->imagePath + temp.str() + ".png");
    }
    double result = test_distorce(img, nloptimg->test_color);
    delete img;
    return result;
}


double lowerR(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(data);
    double test_r = fun(nloptimg->img->theR, x.data());
    return LOWER_LIMIT * nloptimg->img->theR - test_r;
}

double upperR(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(data);
    double test_r = fun(nloptimg->img->theR, x.data());
    return test_r - UPPER_LIMIT * nloptimg->img->theR;
}

double signConstraint(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(data);
    double test_r = fun(nloptimg->img->theR, x.data());
    if (test_sign(nloptimg->img->theR, x.data()) != (d_test_sign(nloptimg->img->theR, x.data()) <= 0)) {
        throw runtime_error("SignConstraintError!!!");
    }
    return test_sign(nloptimg->img->theR, x.data());
}

int main(int argc, char** argstr) {
    string imagePath, outImagePath;
    if (argc == 3) {
        imagePath = argstr[1];
        outImagePath = argstr[2];
    }
    else {
        //cout << "Названия входного - {Название входного, без пробелов и с расширением}:\n";
        cout << "Wrong input\n";
        return 0;
    }

    Color test_color(255, 0, 255);

    nlopt::opt opt(nlopt::LN_COBYLA, 3);
    std::vector<double> lb(3);
    lb[0] = 0; lb[1] = -10; lb[2] = -0.1;
    opt.set_lower_bounds(lb);
    std::vector<double> ub(3);
    ub[0] = 20; ub[1] = 5; ub[2] = 0.01;
    opt.set_upper_bounds(ub);
    NloptImage* data = new NloptImage(imagePath, test_color);
    opt.set_min_objective(myvfunc, data);

    opt.add_inequality_constraint(lowerR, data);
    opt.add_inequality_constraint(upperR, data);
    opt.add_inequality_constraint(signConstraint, data);
    opt.set_xtol_rel(1e-8);
    std::vector<double> x(3);
    x[0] = 1; x[1] = 0; x[2] = 0;
    double minf;

    try {
        nlopt::result result = opt.optimize(x, minf);
        std::cout << "found minimum at f(" << x[0] << "," << x[1] << "," << x[2] << ") = "
            << std::setprecision(10) << minf << std::endl;
    }
    catch (std::exception& e) {
        std::cout << "nlopt failed: " << e.what() << std::endl;
    }

    return 0;
}