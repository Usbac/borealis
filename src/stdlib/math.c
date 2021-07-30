#include <math.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "math.h"


void stdCos(struct result_list *args)
{
    pushResultD(cos(getValueD(args->first)));
}


void stdAcos(struct result_list *args)
{
    pushResultD(acos(getValueD(args->first)));
}


void stdCosh(struct result_list *args)
{
    pushResultD(cosh(getValueD(args->first)));
}


void stdAcosh(struct result_list *args)
{
    pushResultD(acosh(getValueD(args->first)));
}


void stdSin(struct result_list *args)
{
    pushResultD(sin(getValueD(args->first)));
}


void stdAsin(struct result_list *args)
{
    pushResultD(asin(getValueD(args->first)));
}


void stdSinh(struct result_list *args)
{
    pushResultD(sinh(getValueD(args->first)));
}


void stdAsinh(struct result_list *args)
{
    pushResultD(asinh(getValueD(args->first)));
}


void stdTan(struct result_list *args)
{
    pushResultD(tan(getValueD(args->first)));
}


void stdAtan(struct result_list *args)
{
    pushResultD(atan(getValueD(args->first)));
}


void stdTanh(struct result_list *args)
{
    pushResultD(tanh(getValueD(args->first)));
}


void stdAtanh(struct result_list *args)
{
    pushResultD(atanh(getValueD(args->first)));
}


void stdSqrt(struct result_list *args)
{
    pushResultD(sqrt(getValueD(args->first)));
}


void stdLog(struct result_list *args)
{
    pushResultD(log(getValueD(args->first)));
}


void stdAbs(struct result_list *args)
{
    pushResultD(fabs(getValueD(args->first)));
}


void stdFloor(struct result_list *args)
{
    pushResultD(floor(getValueD(args->first)));
}


void stdCeil(struct result_list *args)
{
    pushResultD(ceil(getValueD(args->first)));
}


void stdRound(struct result_list *args)
{
    double n = getValueD(args->first);
    size_t dec = args->first->next != NULL ? getValueD(args->first->next) : 0;
    double decimals = pow(10, dec);
    n *= decimals;
    n = (n >= floor(n) + 0.5) ? ceil(n) : floor(n);

    pushResultD(n / decimals);
}
