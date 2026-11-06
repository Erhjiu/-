#include "support.h"

#include <cassert>

constexpr int N = 5;
constexpr int TOTAL = 10;

enum {
    EMPTY,
    FULL,
    MUTEX
};

struct AndSemaphore {
    int value[3] = {N, 0, 1};
    Lock guard;
    Condition changed;

    void Swait(int a, int b) {
        assert(a >= EMPTY && a <= MUTEX);
        assert(b >= EMPTY && b <= MUTEX && a != b);
        guard.lock();

        while (value[a] < 1 || value[b] < 1) {
            changed.block(guard);
        }

        --value[a];
        --value[b];
        guard.unlock();
    }

    void Ssignal(int a, int b) {
        assert(a >= EMPTY && a <= MUTEX);
        assert(b >= EMPTY && b <= MUTEX && a != b);
        guard.lock();

        ++value[a];
        ++value[b];
        changed.wakeup_all();

        guard.unlock();
    }
};

struct Shared {
    int buffer[N]{};
    int in = 0;
    int out = 0;
    AndSemaphore sem;
};

void producer(Shared* s) {
    for (int i = 1; i <= TOTAL; ++i) {
        int nextp = i;
        usleep(100000);

        s->sem.Swait(EMPTY, MUTEX);

        s->buffer[s->in] = nextp;
        s->in = (s->in + 1) % N;
        std::printf("生产者：放入 %d\n", nextp);

        s->sem.Ssignal(MUTEX, FULL);
    }
}

void consumer(Shared* s) {
    for (int i = 1; i <= TOTAL; ++i) {
        s->sem.Swait(FULL, MUTEX);

        int nextc = s->buffer[s->out];
        s->out = (s->out + 1) % N;
        assert(nextc == i);
        std::printf("消费者：取出 %d\n", nextc);

        s->sem.Ssignal(MUTEX, EMPTY);

        usleep(200000);
    }
}

int main() {
    Shared* s = create_shared<Shared>();
    run_processes(s, producer, consumer);

    assert(s->sem.value[EMPTY] == N);
    assert(s->sem.value[FULL] == 0);
    assert(s->sem.value[MUTEX] == 1);
    std::printf("AND 型：生产、消费完成，验证通过。\n");

    destroy_shared(s);
    return 0;
}
