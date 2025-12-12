#include <stdio.h>

typedef struct {
    int a;
    int b;
} Data;

void update(Data *d) {
    d->a = 10;    // update struct fields through the pointer
    d->b = 20;
}

int main() {
    Data myData = {1, 2};   // initial values

    update(&myData);        // pass address so function can modify it

    printf("a = %d, b = %d\n", myData.a, myData.b);
    return 0;
}
