#include <math.h>
#include "../engine/processor_helper.h"
#include "../state.h"
#include "math.h"


void stdCos(struct result_list *args)
{
    statePushResultD(cos(getValueD(args->first)));
}


void stdAcos(struct result_list *args)
{
    statePushResultD(acos(getValueD(args->first)));
}


void stdCosh(struct result_list *args)
{
    statePushResultD(cosh(getValueD(args->first)));
}


void stdAcosh(struct result_list *args)
{
    statePushResultD(acosh(getValueD(args->first)));
}


void stdSin(struct result_list *args)
{
    statePushResultD(sin(getValueD(args->first)));
}


void stdAsin(struct result_list *args)
{
    statePushResultD(asin(getValueD(args->first)));
}


void stdSinh(struct result_list *args)
{
    statePushResultD(sinh(getValueD(args->first)));
}


void stdAsinh(struct result_list *args)
{
    statePushResultD(asinh(getValueD(args->first)));
}


void stdTan(struct result_list *args)
{
    statePushResultD(tan(getValueD(args->first)));
}


void stdAtan(struct result_list *args)
{
    statePushResultD(atan(getValueD(args->first)));
}


void stdTanh(struct result_list *args)
{
    statePushResultD(tanh(getValueD(args->first)));
}


void stdAtanh(struct result_list *args)
{
    statePushResultD(atanh(getValueD(args->first)));
}


void stdSqrt(struct result_list *args)
{
    statePushResultD(sqrt(getValueD(args->first)));
}


void stdLog(struct result_list *args)
{
    statePushResultD(log(getValueD(args->first)));
}


void stdAbs(struct result_list *args)
{
    statePushResultD(fabs(getValueD(args->first)));
}


void stdFloor(struct result_list *args)
{
    statePushResultD(floor(getValueD(args->first)));
}


void stdCeil(struct result_list *args)
{
    statePushResultD(ceil(getValueD(args->first)));
}


void stdRound(struct result_list *args)
{
    double n = getValueD(args->first);
    size_t dec = args->first->next != NULL ? getValueD(args->first->next) : 0;
    double decimals = pow(10, dec);
    n *= decimals;
    n = (n >= floor(n) + 0.5) ? ceil(n) : floor(n);

    statePushResultD(n / decimals);
}
