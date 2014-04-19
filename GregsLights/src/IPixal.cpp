/*
 * Defines a single light (or pixal) that can have its intensity independantly
 * adjusted. Intensity is always passed as a percentage because DMX and LOR have
 * different measures
 */

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
