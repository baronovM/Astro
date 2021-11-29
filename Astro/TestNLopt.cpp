#include <iomanip>
#include <iostream>
#include <vector>

#include "Astro.h"

#include <nlopt.hpp>

double myfunc(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
{
    return sqr(1 - x[0]) + 100 * sqr(x[1] - x[0] * x[0]);
}

double myvconstraint(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    //my_constraint_data* d = reinterpret_cast<my_constraint_data*>(data);
    //double a = d->a, b = d->b;
    //if (!grad.empty())
    return ;
}

int main() {
    nlopt::opt opt(nlopt::LN_NELDERMEAD, 2);
    std::vector<double> lb(2);
    lb[0] = -10; lb[1] = -10;
    opt.set_lower_bounds(lb);
    std::vector<double> ub(2);
    ub[0] = 10; ub[1] = 10;
    opt.set_upper_bounds(ub);
    opt.set_min_objective(myfunc, NULL);
    opt.set_xtol_rel(1e-4);
    std::vector<double> x(2);
    x[0] = -2.33; x[1] = 8.5;
    double minf;

    try {
        nlopt::result result = opt.optimize(x, minf);
        std::cout << "found minimum at f(" << x[0] << "," << x[1] << ") = "
            << std::setprecision(10) << minf << std::endl;
    }
    catch (std::exception& e) {
        std::cout << "nlopt failed: " << e.what() << std::endl;
    }

    return 0;
}