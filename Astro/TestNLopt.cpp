#include <nlopt.hpp>
#include "Astro.h"

double myvfunc(const std::vector<double>& x, std::vector<double>& grad, void* my_func_data)
{
    return sqr(1 - x[0]) + 100 * sqr(x[1] - x[0] * x[0]);
}

struct my_constraint_data {
    double a, b;
};

double myvconstraint(const std::vector<double>& x, std::vector<double>& grad, void* data)
{
    my_constraint_data* d = reinterpret_cast<my_constraint_data*>(data);
    double a = d->a, b = d->b;

    return 5 - x[1];
}

int main() {
    nlopt::opt opt(nlopt::LN_COBYLA, 2);
    std::vector<double> lb(2);
    lb[0] = -10; lb[1] = -10;
    opt.set_lower_bounds(lb);
    std::vector<double> ub(2);
    ub[0] = 10; ub[1] = 10;
    opt.set_upper_bounds(ub);
    opt.set_min_objective(myvfunc, NULL);
    my_constraint_data data[2] = { { 2, 0 }, { -1, 1 } };

    opt.add_inequality_constraint(myvconstraint, &data[0]);
    opt.add_inequality_constraint(myvconstraint, &data[1]);
    opt.set_xtol_rel(1e-8);
    std::vector<double> x(2);
    x[0] = 1.234; x[1] = 5.678;
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