#include "Sampler/BoundedSpscQueue.h"

#include <atomic>
#include <iostream>
#include <thread>

int main()
{
    ppgu::sampler::BoundedSpscQueue<int, 8> queue;
    for (int value = 0; value < 7; ++value)
        if (!queue.tryPush(value))
            return 1;
    if (queue.tryPush(7))
        return 2;
    for (int expected = 0; expected < 7; ++expected)
    {
        int value = -1;
        if (!queue.tryPop(value) || value != expected)
            return 3;
    }
    int emptyValue = 0;
    if (queue.tryPop(emptyValue) || !queue.empty())
        return 4;

    // Repeated wrap-around must preserve FIFO ordering.
    for (int pass = 0; pass < 1000; ++pass)
    {
        if (!queue.tryPush(pass))
            return 5;
        int value = -1;
        if (!queue.tryPop(value) || value != pass)
            return 6;
    }

    // Exercise the actual single-producer/single-consumer concurrency model
    // used by the audio callback and the sample streaming worker.
    constexpr int itemCount = 250000;
    std::atomic<bool> producerDone { false };
    std::atomic<bool> failed { false };
    std::thread producer([&]
    {
        for (int value = 0; value < itemCount; ++value)
            while (!queue.tryPush(value))
                std::this_thread::yield();
        producerDone.store(true, std::memory_order_release);
    });

    int expected = 0;
    while (!producerDone.load(std::memory_order_acquire) || !queue.empty())
    {
        int value = -1;
        if (queue.tryPop(value))
        {
            if (value != expected)
                failed.store(true, std::memory_order_relaxed);
            ++expected;
        }
        else
            std::this_thread::yield();
    }
    producer.join();

    if (failed.load(std::memory_order_relaxed) || expected != itemCount)
        return 7;

    std::cout << "SPSC queue contracts passed: capacity, FIFO, wrap-around, concurrency.\n";
    return 0;
}
