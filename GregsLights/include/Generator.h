#ifndef Generator_H
#define Generator_H

#include <vector>
#include<string>

using namespace std;

template <class T>
class Generator
{
public:
    Generator();
    virtual ~Generator();
    void add(T s);
    T getRandom();
protected:
private:
    void clear();
    void checkClear();
    vector<T> allStrings;
    vector<bool> usemap;
    size_t cnt=0;
};

class MovieInfo
{
public:
    MovieInfo(string name, int count, double duration);
    virtual ~MovieInfo();
    string name;
    int count;
    double duration;
};

#endif // Generator_H
