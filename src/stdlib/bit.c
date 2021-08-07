#include "../engine/processor_helper.h"
#include "../state.h"
#include "bit.h"


static int getBits(struct result *result)
{
    return (int) getValueD(result);
}


void stdAnd(struct result_list *args)
{
    statePushResultD((double) (getBits(args->first) & getBits(args->first->next)));
}


void stdOr(struct result_list *args)
{
    statePushResultD((double) (getBits(args->first) | getBits(args->first->next)));
}


void stdLeftShift(struct result_list *args)
{
    statePushResultD((double) (getBits(args->first) << getBits(args->first->next)));
}


void stdRightShift(struct result_list *args)
{
    statePushResultD((double) (getBits(args->first) >> getBits(args->first->next)));
}


void stdNot(struct result_list *args)
{
    statePushResultD((double) ~getBits(args->first));
}
