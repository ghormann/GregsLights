#include <iostream>
#include "include/Bush.h"
#include "include/IPixal.h"
#include "include/DummyPixal.h"

using namespace std;

int main()
{
    cout << "Hello world!!!" << endl;
    Bush *bush1 = new Bush(10, new DummyPixal(2), new DummyPixal(3), new DummyPixal(4), new DummyPixal(5));
    cout << "Bush: " << bush1->getId() << endl;
    bush1->setAll(100);
    cout << "-----------------------------------------" << endl;
    bush1->setAll(0);
    cout << "-----------------------------------------" << endl;
    bush1->setAll(10);
    return 0;
}
