#include "MBEKlib.h"


double myvfunc(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
{
    const NloptImage* nloptimg = static_cast<NloptImage*>(my_func_data);

    ostringstream temp;
    for (int i = 0; i < NUMCOEF; ++i)
        temp << "_" << x[i];
    SmartImage* img;
    if (!filesystem::exists("out/" + nloptimg->imagePath + temp.str() + ".png")) {
        img = distorce(nloptimg->img, x.data());
        img->saveToFile("out/" + nloptimg->imagePath + temp.str() + ".png");
    }
    else {
        img = new SmartImage("out/" + nloptimg->imagePath + temp.str() + ".png");
    }
    double result = test_distorce(img, nloptimg->test_color);
    delete img;
    cout << "f(" << x[0] << " " << x[1] << " " << x[2] << ") = " << result << "\n";
    return result;
}

double cntConstraint(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
{
    const NloptImage* nloptimg = static_cast<NloptImage*>(my_func_data);

    ostringstream temp;
    for (int i = 0; i < NUMCOEF; ++i)
        temp << "_" << x[i];

    SmartImage* img;
    if (!filesystem::exists("out/" + nloptimg->imagePath + temp.str() + ".png")) {
        img = distorce(nloptimg->img, x.data());
        img->saveToFile("out/" + nloptimg->imagePath + temp.str() + ".png");
    }
    else {
        img = new SmartImage("out/" + nloptimg->imagePath + temp.str() + ".png");
    }

    int cnt = 0;
    vector<Vector2u> coords;
    for (int x = 0; x < img->getSize().x; x++) {
        for (int y = 0; y < img->getSize().y; y++) {
            if (NumColor(img->getPixel(x, y)) - NumColor(nloptimg->test_color) < THRESHOLD) {
                ++cnt;
            }
        }
    }
    cout << "cntConstr(" << x[0] << " " << x[1] << " " << x[2] << ") = " << cnt - 5 << "\n";
    return cnt - 5;
}

double lowerR(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    if (isnan(x[0])) {
        cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n";
    }
    const NloptImage* nloptimg = static_cast<NloptImage*>(data);
    double test_r = func(nloptimg->img->theR, x.data());
    if (isnan(-(test_r - LOWER_LIMIT * nloptimg->img->theR))) {
        cout << "lower nan, testr = " << test_r << "; " << LOWER_LIMIT << "; " << nloptimg->img->theR << "; x " << x[0] << " " << x[1] << " " << x[2] << "\n";
    }
    cout << "lowerR constraint(" << x[0] << " " << x[1] << " " << x[2] << ") = " << (test_r - LOWER_LIMIT * nloptimg->img->theR) <<"\n";
    return test_r - LOWER_LIMIT * nloptimg->img->theR;
}

double upperR(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = static_cast<NloptImage*>(data);
    double test_r = func(nloptimg->img->theR, x.data());
    if (isnan(-(UPPER_LIMIT * nloptimg->img->theR - test_r))) {
        cout << "upper nan, testr =  " << test_r << "; " << UPPER_LIMIT << "; " << nloptimg->img->theR << "; x " << x[0] << " " << x[1] << " " << x[2] << "\n";
    }
    cout << "upperR constraint(" << x[0] << " " << x[1] << " " << x[2] << ") = " << (UPPER_LIMIT * nloptimg->img->theR - test_r) << "\n";
    return UPPER_LIMIT * nloptimg->img->theR - test_r;
}

double signConstraint(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    const NloptImage* nloptimg = static_cast<NloptImage*>(data);

    if (isnan(-cont_test_sign(nloptimg->img->theR, x.data()))) {
        cout << "sign nan, ther =  " << nloptimg->img->theR << "; " << cont_test_sign(nloptimg->img->theR, x.data()) << "; x " << x[0] << " " << x[1] << " " << x[2] << "\n";
    }
    cout << "sign constraint(" << x[0] << " " << x[1] << " " << x[2] << ") = " << cont_test_sign(nloptimg->img->theR, x.data()) << "\n";
    return cont_test_sign(nloptimg->img->theR, x.data());
}