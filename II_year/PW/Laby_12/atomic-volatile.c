#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>

atomic_int a = 0;
volatile atomic_int v = 0;

void* f(void* arg) {
    (void)arg; // Unused argument
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&a, 1);
        v++;
    }
    return NULL;
}

int main() {
    printf("main() starts\n");
    pthread_t t1, t2;

    pthread_create(&t1, NULL, f, NULL);
    pthread_create(&t2, NULL, f, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("main() completes: v=%d a=%d\n", v, atomic_load(&a));

    return 0;
}
