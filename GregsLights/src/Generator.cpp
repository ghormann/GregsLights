#include "../include/Generator.h"

using namespace std;

template <class T>
Generator<T>::Generator()
{
    //ctor
}


template <class T>
void Generator<T>::add(T s)
{
    allStrings.push_back(s);
    usemap.push_back(false);
    ++cnt;
}

template <class T>
T Generator<T>::getRandom()
{
    checkClear();
    size_t i = 0;
    while (usemap[i] == true)
    {
        i = rand() % cnt;  // Don't show same message twice in a row.
    }
    usemap[i] = true;

    return allStrings[i];

}

template <class T>
void Generator<T>::checkClear()
{
    for (size_t i = 0; i < cnt; i++)
    {
        if (usemap[i] == false) return;
    }
    // If we got this far, everything is used up.  Clear it.
    this->clear();
}

template <class T>
void Generator<T>::clear()
{
    for (size_t i = 0; i < cnt; i++)
    {
        usemap[i] = false;
    }

}


template <class T>
Generator<T>::~Generator()
{
    //dtor
}

MovieInfo::~MovieInfo()
{
}

MovieInfo::MovieInfo(string name, int count, double duration)
{
    this->name = name;
    this->count = count;
    this->duration = duration;
}

template class Generator<std::string>;
template class Generator<MovieInfo>;
