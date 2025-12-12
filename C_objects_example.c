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


-----------------------
#include <stdio.h>

typedef struct {
    int a;
    int b;
} Data;

void step(Data *d) {
    d->a += 1;   // add 1 to each field
    d->b += 1;
}

int main() {
    Data myData = {0, 0};

    for (int i = 0; i < 5; i++) {
        step(&myData);   // update struct through pointer
        printf("Cycle %d: a = %d, b = %d\n", i, myData.a, myData.b);
    }

    return 0;
}


