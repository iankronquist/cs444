#include <assert.h>

#include "_producer-consumer.c"

int main()
{
    random_number_init();

    assert(BufferFill == 0);
    _producer();
    assert(pthread_mutex_trylock(&BufferLock) == 0);
    assert(BufferFill == 1);
    assert(Buffer[0].wait_period >= 2);
    assert(Buffer[0].wait_period < 9);
    struct item zero = Buffer[0];
    pthread_mutex_unlock(&BufferLock);

    _producer();
    assert(pthread_mutex_trylock(&BufferLock) == 0);
    assert(BufferFill == 2);
    assert(Buffer[1].wait_period >= 2);
    assert(Buffer[1].wait_period < 9);
    // The zero item did not change
    assert(Buffer[0].value == zero.value);
    assert(Buffer[0].wait_period == zero.wait_period);
    pthread_mutex_unlock(&BufferLock);

    _consumer();
    assert(pthread_mutex_trylock(&BufferLock) == 0);
    assert(BufferFill == 1);
    assert(Buffer[0].value == zero.value);
    assert(Buffer[0].wait_period == zero.wait_period);
    pthread_mutex_unlock(&BufferLock);

    _consumer();
    assert(pthread_mutex_trylock(&BufferLock) == 0);
    assert(BufferFill == 0);
    pthread_mutex_unlock(&BufferLock);

    return 0;
}
