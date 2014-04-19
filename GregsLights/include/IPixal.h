#ifndef IPIXAL_H
#define IPIXAL_H


class IPixal
{
    public:
        virtual int getMin() = 0;
        virtual int getMax() = 0;
        void setIntensity(int pct);
    protected:
        virtual void setIntensity_ipml(int pct) = 0;

    private:
};

#endif // IPIXAL_H
