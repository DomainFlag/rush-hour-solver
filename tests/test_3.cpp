#include <iostream>

#include "./../include/RushHour.h"
#include "./../include/Window.h"

using namespace std;

int main(int argc, char ** argv) {
    srand(time(NULL));

    RushHour::create(6, 3, 0, 2, 1, 3);

    return 0;
};
