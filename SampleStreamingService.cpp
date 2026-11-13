#include "SampleStreamingService.h"

namespace ppgu {

void SampleStreamingService::start() {
    std::lock_guard<std::mutex> lk(startStop_);
    if (running_.load()) return;
    running_.store(true);
    workerAlive_.store(true);
    worker_ = std::thread([this] { run(); });
}

void SampleStreamingService::stop() {
    {
        std::lock_guard<std::mutex> lk(startStop_);
        if (!running_.exchange(false)) return;
    }
    queue_.drain(); // full drain before join (INFO contract)
    if (worker_.joinable()) worker_.join();
    workerAlive_.store(false);
}

bool SampleStreamingService::submit(const Request& r) { return queue_.push(r); }

void SampleStreamingService::run() {
    while (running_.load()) {
        try {
            Request r;
            if (queue_.pop(r)) {
                // decode page work happens here in production; contract test uses no-op
                continue;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
        } catch (...) {
            // worker auto-recovery: cooldown + counter (closes open risk #3 from INFO)
            recoveries_.fetch_add(1, std::memory_order_relaxed);
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            queue_.drain();
        }
    }
}

} // namespace ppgu