#include "support.h"

#include <cassert>

using namespace std;

constexpr int N = 5;
constexpr int TOTAL = 10;
constexpr int PRODUCER = 0;
constexpr int CONSUMER = 1;

struct Semaphore {
    int value;
    int L[2]{};
    int head = 0;
    int tail = 0;
    bool granted[2]{};

    Lock guard;
    Condition blocked[2];

    explicit Semaphore(int initial) : value(initial) {}

    void wait(int id) {
        guard.lock();

        --value;
        if (value < 0) {
            L[tail] = id;
            tail = (tail + 1) % 2;

            while (!granted[id]) {
                blocked[id].block(guard);
            }
            granted[id] = false;
        }

        guard.unlock();
    }

    void signal() {
        guard.lock();

        ++value;
        if (value <= 0) {
            int id = L[head];
            head = (head + 1) % 2;
            granted[id] = true;
            blocked[id].wakeup();
        }

        guard.unlock();
    }
};

struct Shared {
    int buffer[N]{};
    int in = 0;
    int out = 0;

    Semaphore empty{N};
    Semaphore full{0};
    Semaphore mutex{1};
};

void producer(Shared* s) {
    for (int i = 1; i <= TOTAL; ++i) {
        int nextp = i;
        usleep(100000);

        s->empty.wait(PRODUCER);
        s->mutex.wait(PRODUCER);

        s->buffer[s->in] = nextp;
        s->in = (s->in + 1) % N;
        std::printf("生产者：放入 %d\n", nextp);

        s->mutex.signal();
        s->full.signal();
    }
}

void consumer(Shared* s) {
    for (int i = 1; i <= TOTAL; ++i) {
        s->full.wait(CONSUMER);
        s->mutex.wait(CONSUMER);

        int nextc = s->buffer[s->out];
        s->out = (s->out + 1) % N;
        assert(nextc == i);
        std::printf("消费者：取出 %d\n", nextc);

        s->mutex.signal();
        s->empty.signal();

        usleep(200000);
    }
}

int main() {
    Shared* s = create_shared<Shared>();
    run_processes(s, producer, consumer);

    assert(s->empty.value == N);
    assert(s->full.value == 0);
    assert(s->mutex.value == 1);
    std::printf("记录型：生产、消费完成，验证通过。\n");

    destroy_shared(s);
    return 0;
}
