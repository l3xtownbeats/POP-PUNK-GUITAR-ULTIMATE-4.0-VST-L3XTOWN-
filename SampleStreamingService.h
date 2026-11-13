#pragma once
#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include "BoundedSpscQueue.h"

namespace ppgu {

// Off-audio-thread decoding service. Worker auto-recovers after exception
// (cooldown 250 ms + recovery counter). Full drain on stop. No <memory> missing.
class SampleStreamingService {
public:
    struct Request { uint64_t sampleId; uint64_t firstFrame; };
    explicit SampleStreamingService(size_t queueCapacity = 256) : queue_(queueCapacity) {}
    ~SampleStreamingService() { stop(); }
    void start();
    void stop(); // drains queue before join
    bool submit(const Request& r);
    uint64_t overflowCount() const { return queue_.overflowCount(); }
    uint32_t workerRecoveries() const { return recoveries_.load(std::memory_order_relaxed); }
    bool workerAlive() const { return workerAlive_.load(std::memory_order_relaxed); }
    void drainQueue() { queue_.drain(); }
private:
    void run();
    BoundedSpscQueue<Request> queue_;
    std::thread worker_;
    std::atomic<bool> running_{false};
    std::atomic<bool> workerAlive_{false};
    std::atomic<uint32_t> recoveries_{0};
    std::mutex startStop_;
};

} // namespace ppgu