#include "../include/IPixal.h"

void IPixal::setIntensity(int pct)
{
    int value = 0;
    if (pct >= 100)
    {
        setIntensity_ipml(getMax());
    }
    else if (pct <= 0)
    {
        setIntensity_ipml(getMin());
    }
    else
    {
        value = ((getMax() - getMin()) * pct)/100;
        setIntensity_ipml(value);
    }
}
