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


double myvfunc(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(my_func_data);

    ostringstream temp;
    for (int i = 0; i < NUMCOEF; ++i)
        temp << "__" << x[i];
    SmartImage* img;
    if (!filesystem::exists("out/" + nloptimg->imagePath + temp.str() + ".png")) {
        img = distorce(nloptimg->img, x.data());
    }
    else {
        img = new SmartImage("out/" + nloptimg->imagePath + temp.str() + ".png");
    }
    double result = test_distorce(img, nloptimg->test_color);
    delete img;
    return result;
}


double lowerR(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(data);
    double test_r = func(nloptimg->img->theR, x.data());
    return -(test_r - LOWER_LIMIT * nloptimg->img->theR);
}

double upperR(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(data);
    double test_r = func(nloptimg->img->theR, x.data());
    return -(UPPER_LIMIT * nloptimg->img->theR - test_r);
}

double signConstraint(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = reinterpret_cast<NloptImage*>(data);
    return -cont_test_sign(nloptimg->img->theR, x.data());
}

int main(int argc, char** argstr) {
    srand(time(0));
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

    Color test_color;
    try {
        ifstream fin("color.txt");
        fin >> test_color.r >> test_color.g >> test_color.b;
        fin.close();
    }
    catch (exception& e) {
        test_color = { 255, 0, 255 };
    }


    nlopt::opt opt(nlopt::LN_COBYLA, 3);
    vector<double> lb(3);
    lb[0] = 0; lb[1] = -12; lb[2] = -0.2;
    opt.set_lower_bounds(lb);
    vector<double> ub(3);
    ub[0] = 10; ub[1] = 7; ub[2] = 0.02;
    opt.set_upper_bounds(ub);
    NloptImage* data = new NloptImage(imagePath, test_color);
    opt.set_min_objective(myvfunc, data);

    opt.add_inequality_constraint(lowerR, data);
    opt.add_inequality_constraint(upperR, data);
    opt.add_inequality_constraint(signConstraint, data);
    opt.set_xtol_rel(1e-8);
    vector<double> x(3);
    x[0] = 1; x[1] = 0; x[2] = 0;
    //x[0] = double(rand() % 200) / 10; x[1] = double(rand() % 150) / 10 - 10; x[2] = double(rand() % 11) / 100 - 0.1;
    double minf;
    cout << x[0] << " " << x[1] << " " << x[2] << "\n";
    cout << lowerR(x, x, data) << " " << upperR(x, x, data) << " " << signConstraint(x, x, data) << "\n";

    try {
        nlopt::result result = opt.optimize(x, minf);
        cout << "found minimum at f(" << x[0] << "," << x[1] << "," << x[2] << ") = "
            << setprecision(10) << minf << endl;

        cout << lowerR(x, x, data) << " " << upperR(x, x, data) << " " << signConstraint(x, x, data) << "\n";

        ofstream fout("coef.txt");
        for (auto i : x) {
            fout << i << " ";
        }
        fout.close();

        ostringstream temp;
        for (int i = 0; i < NUMCOEF; ++i)
            temp << "__" << x[i];
        SmartImage* img;
        if (!filesystem::exists("out/" + data->imagePath + temp.str() + ".png")) {
            img = distorce(data->img, x.data());
        }
        else {
            img = new SmartImage("out/" + data->imagePath + temp.str() + ".png");
        }
        img->saveToFile(outImagePath);

    }
    catch (std::exception& e) {
        std::cout << "nlopt failed: " << e.what() << std::endl;
    }

    return 0;
}