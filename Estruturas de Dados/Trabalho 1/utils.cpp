#include "utils.h"
#include <fstream>
#include <stdlib.h>
#include <string>
using namespace std;

void readFile(string path, const function<void(string)> &forEachLineCallback) {
  string line;
  ifstream file(path);

  if (!file.is_open()) {
    cout << "Unable to open file" << endl;
    exit(1);
  }

  while (getline(file, line))
    forEachLineCallback(line);

  file.close();
}
