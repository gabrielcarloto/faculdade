#include <functional>
#include <iostream>
using namespace std;

void readFile(string path, const function<void(string)> &forEachLineCallback);
