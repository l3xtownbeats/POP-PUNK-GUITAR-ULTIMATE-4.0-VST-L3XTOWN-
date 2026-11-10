#include "BoundedSpscQueue.h"

#include <atomic>
#include <cstdint>
#include <iostream>
#include <thread>

struct Request
{
    std::uint32_t sampleId = 0;
    std::uint32_t pageIndex = 0;
};

int main()
{
    using Queue = ppgu::sampler::BoundedSpscQueue<Request, 8>;
    Queue queue;
    if (Queue::usableCapacity() != 7 || !queue.empty())
        return 1;

    for (std::uint32_t i = 0; i < 7; ++i)
        if (!queue.tryPush({ i, i * 3 }))
            return 1;
    if (queue.tryPush({ 99, 99 }))
    {
        std::cerr << "Queue did not reject a full request buffer.\n";
        return 1;
    }

    for (std::uint32_t i = 0; i < 7; ++i)
    {
        Request value;
        if (!queue.tryPop(value) || value.sampleId != i || value.pageIndex != i * 3)
            return 1;
    }
    if (!queue.empty())
        return 1;

    // Exercise index wrap-around and FIFO ordering repeatedly.
    for (std::uint32_t round = 0; round < 1000; ++round)
    {
        for (std::uint32_t i = 0; i < 5; ++i)
            if (!queue.tryPush({ round, i }))
                return 1;
        for (std::uint32_t i = 0; i < 5; ++i)
        {
            Request value;
            if (!queue.tryPop(value) || value.sampleId != round || value.pageIndex != i)
                return 1;
        }
    }

    // SPSC stress: producer and consumer transfer a long monotonically
    // increasing sequence. A full queue is a normal bounded back-pressure case.
    Queue concurrentQueue;
    constexpr std::uint32_t total = 500000;
    std::atomic<bool> failed { false };
    std::thread producer([&]
    {
        for (std::uint32_t i = 0; i < total;)
        {
            if (concurrentQueue.tryPush({ i, i ^ 0x5a5aU }))
                ++i;
            else
                std::this_thread::yield();
        }
    });
    std::thread consumer([&]
    {
        for (std::uint32_t expected = 0; expected < total;)
        {
            Request value;
            if (concurrentQueue.tryPop(value))
            {
                if (value.sampleId != expected || value.pageIndex != (expected ^ 0x5a5aU))
                    failed.store(true, std::memory_order_relaxed);
                ++expected;
            }
            else
                std::this_thread::yield();
        }
    });
    producer.join();
    consumer.join();

    if (failed.load(std::memory_order_relaxed) || !concurrentQueue.empty())
    {
        std::cerr << "Concurrent SPSC stress test failed.\n";
        return 1;
    }

    std::cout << "Bounded SPSC queue tests passed: capacity, full rejection, FIFO, wrap-around and concurrent transfer.\n";
    return 0;
}
