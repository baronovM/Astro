#include "Astro.h"
#include <filesystem>
#include <memory>
#include "gsl/gsl_multimin.h"

class GslImage : public PlanImage {
public:
	Color test_color;
	GslImage(string filePath, const Color& c) : PlanImage(filePath) {
		test_color = c;
	}
};

double gsl_distorce(const gsl_vector* x, void* params) {
	GslImage* img = (GslImage*)params;
	return test_distorce(*distorce(*(PlanImage*)img, x->data), img->test_color);
}

int main(int argc, char** argstr) {
	string imagePath, outImagePath;
	if (argc == 3) {
		imagePath = argstr[1];
		outImagePath = argstr[2];
	}
	else
		cout << "Названия входного и выходного изображений - {Название входного, без пробелов и с расширением} {Название выходного}:\n";

	Color test_color(255, 0, 255);
	GslImage* inImage = new GslImage(imagePath, test_color);

    const gsl_multimin_fminimizer_type* T =
        gsl_multimin_fminimizer_nmsimplex2;
    gsl_multimin_fminimizer* s = NULL;
    gsl_vector* ss, * x;
    gsl_multimin_function minex_func;

    size_t iter = 0;
    int status;
    double size;

    /* Starting point */
    x = gsl_vector_alloc(3);
    gsl_vector_set(x, 0, 2.5);
    gsl_vector_set(x, 1, 0.0);
    gsl_vector_set(x, 2, 0.0);

    /* Set initial step sizes to 1 */
    ss = gsl_vector_alloc(3);
    gsl_vector_set_all(ss, 1.0);

    /* Initialize method and iterate */
    minex_func.n = 3;
    minex_func.f = gsl_distorce;
    minex_func.params = (void*)inImage;

    s = gsl_multimin_fminimizer_alloc(T, 3);
    gsl_multimin_fminimizer_set(s, &minex_func, x, ss);

    do
    {
        iter++;
        status = gsl_multimin_fminimizer_iterate(s);

        if (status)
            break;

        size = gsl_multimin_fminimizer_size(s);
        status = gsl_multimin_test_size(size, 1e-2);

        if (status == GSL_SUCCESS)
        {
            printf("converged to minimum at\n");
        }

        printf("%5d %10.3e %10.3e %10.3e f() = %7.3f size = %.3f\n",
            int(iter),
            gsl_vector_get(s->x, 0),
            gsl_vector_get(s->x, 1),
            gsl_vector_get(s->x, 2),
            s->fval, size);
    } while (status == GSL_CONTINUE && iter < 100);

    gsl_vector_free(x);
    gsl_vector_free(ss);
    gsl_multimin_fminimizer_free(s);

    return status;

}
