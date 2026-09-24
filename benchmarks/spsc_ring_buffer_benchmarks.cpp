#include "saa/data_structures/spsc_ring_buffer.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <thread>

namespace {

void BM_ProducerConsumerThroughput(benchmark::State& state) {
    constexpr std::size_t item_count = 1'000'000;
    const auto capacity = static_cast<std::size_t>(state.range(0));
    saa::SpscRingBuffer<std::uint64_t> buffer(capacity);

    for (auto _ : state) {
        std::thread producer([&buffer, item_count] {
            for (std::size_t i = 0; i < item_count; ++i) {
                while (!buffer.push(static_cast<std::uint64_t>(i))) {
                    std::this_thread::yield();
                }
            }
        });

        std::uint64_t checksum = 0;
        for (std::size_t received = 0; received < item_count;) {
            auto value = buffer.pop();
            if (!value) {
                std::this_thread::yield();
                continue;
            }

            checksum += *value;
            ++received;
        }

        producer.join();
        benchmark::DoNotOptimize(checksum);
    }

    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(item_count));
}

BENCHMARK(BM_ProducerConsumerThroughput)
    ->Arg(16)
    ->Arg(64)
    ->Arg(256)
    ->Arg(1024)
    ->Arg(4096)
    ->Arg(16384)
    ->UseRealTime();

} // namespace

BENCHMARK_MAIN();
