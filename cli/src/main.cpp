#include <cstdio>

#include "KitsuneEngine/grabber.h"

int main() {
    auto const g = new grabber();
    printf(g->hello().c_str());
    return 0;
}