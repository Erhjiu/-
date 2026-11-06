#pragma once

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <new>

#include <pthread.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

inline void check_pthread(int result) {
    if (result != 0) {
        errno = result;
        std::perror("pthread");
        std::exit(1);
    }
}

inline void system_error(const char* message) {
    std::perror(message);
    std::exit(1);
}

struct Lock {
    pthread_mutex_t native;

    Lock() {
        pthread_mutexattr_t attr;
        check_pthread(pthread_mutexattr_init(&attr));
        check_pthread(pthread_mutexattr_setpshared(
            &attr, PTHREAD_PROCESS_SHARED
        ));
        check_pthread(pthread_mutex_init(&native, &attr));
        check_pthread(pthread_mutexattr_destroy(&attr));
    }

    void lock() {
        check_pthread(pthread_mutex_lock(&native));
    }

    void unlock() {
        check_pthread(pthread_mutex_unlock(&native));
    }

    ~Lock() {
        check_pthread(pthread_mutex_destroy(&native));
    }
};

struct Condition {
    pthread_cond_t native;

    Condition() {
        pthread_condattr_t attr;
        check_pthread(pthread_condattr_init(&attr));
        check_pthread(pthread_condattr_setpshared(
            &attr, PTHREAD_PROCESS_SHARED
        ));
        check_pthread(pthread_cond_init(&native, &attr));
        check_pthread(pthread_condattr_destroy(&attr));
    }

    void block(Lock& lock) {
        check_pthread(pthread_cond_wait(&native, &lock.native));
    }

    void wakeup() {
        check_pthread(pthread_cond_signal(&native));
    }

    void wakeup_all() {
        check_pthread(pthread_cond_broadcast(&native));
    }

    ~Condition() {
        check_pthread(pthread_cond_destroy(&native));
    }
};

template <typename T>
T* create_shared() {
    void* memory = mmap(
        nullptr, sizeof(T),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1, 0
    );

    if (memory == MAP_FAILED) {
        system_error("mmap");
    }

    return new (memory) T{};
}

template <typename T>
void destroy_shared(T* object) {
    object->~T();
    if (munmap(object, sizeof(T)) == -1) {
        system_error("munmap");
    }
}

template <typename T>
void run_processes(
    T* shared,
    void (*producer)(T*),
    void (*consumer)(T*)
) {
    std::setvbuf(stdout, nullptr, _IONBF, 0);

    pid_t pid = fork();
    if (pid == -1) {
        system_error("fork");
    }

    if (pid == 0) {
        producer(shared);
        _exit(0);
    }

    consumer(shared);

    int status = 0;
    pid_t result;
    do {
        result = waitpid(pid, &status, 0);
    } while (result == -1 && errno == EINTR);

    if (result == -1) {
        system_error("waitpid");
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        std::fprintf(stderr, "生产者进程异常退出。\n");
        std::exit(1);
    }
}
