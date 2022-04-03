#include "MBEKlib.h"


int main(int argc, char** argstr) {
    srand(time(0));
    string imagePath, outImagePath;
    if (argc == 3) {
        imagePath = argstr[1];
        outImagePath = argstr[2];
    }
    else {
        cout << "Wrong input\n";
        cout << "Названия входного и выходного\n";
        return 0;
    }

    int tr, tg, tb;
    try {
        ifstream fin("color.txt");
        fin >> tr >> tg >> tb;
        fin.close();
    }
    catch (exception& e) {
        tr = 255;
        tg = 0;
        tb = 255;
    }
    Color test_color(tr, tg, tb);


    nlopt::opt opt(nlopt::GN_AGS, 3);
    vector<double> lb(3);
    lb[0] = 0; lb[1] = -2; lb[2] = -0.02;
    opt.set_lower_bounds(lb);
    vector<double> ub(3);
    ub[0] = 5; ub[1] = 2; ub[2] = 0.02;
    opt.set_upper_bounds(ub);
    NloptImage* data = new NloptImage(imagePath, test_color);
    opt.set_min_objective(myvfunc, data);

    opt.add_inequality_constraint(lowerR, data);
    opt.add_inequality_constraint(upperR, data);
    opt.add_inequality_constraint(signConstraint, data);
    opt.add_inequality_constraint(cntConstraint, data);
    opt.set_xtol_rel(1e-7);
    vector<double> x(3);
    x[0] = 1; x[1] = 0; x[2] = 0;
    double minf;
    cout << test_distorce(data->img, test_color) << "\n";
    cout << x[0] << " " << x[1] << " " << x[2] << "\n";
    cout << lowerR(x, x, data) << " " << upperR(x, x, data) << " " << signConstraint(x, x, data) << " " << cntConstraint(x, x, data) << "\n";

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