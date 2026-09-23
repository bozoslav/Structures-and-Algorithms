#include "saa/data_structures/slot_map.hpp"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace {

void BM_ConstructAndBulkInsert(benchmark::State& state) {
    const auto count = static_cast<std::size_t>(state.range(0));

    for (auto _ : state) {
        saa::SlotMap<std::uint64_t> map(count);
        for (std::size_t i = 0; i < count; ++i) {
            auto key = map.insert(static_cast<std::uint64_t>(i));
            benchmark::DoNotOptimize(key);
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(count));
}
BENCHMARK(BM_ConstructAndBulkInsert)->Arg(64)->Arg(1024)->Arg(16384);

void BM_FetchHit(benchmark::State& state) {
    const auto count = static_cast<std::size_t>(state.range(0));

    saa::SlotMap<std::uint64_t> map(count);
    auto keys = std::make_unique<saa::SlotMap<std::uint64_t>::Key[]>(count);
    for (std::size_t i = 0; i < count; ++i) {
        auto key = map.insert(static_cast<std::uint64_t>(i));
        if (!key) {
            state.SkipWithError("slot map filled before setup completed");
            return;
        }
        keys[i] = *key;
    }
    std::size_t cursor = 0;
    for (auto _ : state) {
        auto* value = map.fetch(keys[cursor]);
        benchmark::DoNotOptimize(value);
        if (value != nullptr) benchmark::DoNotOptimize(*value);
        cursor = (cursor + 1) % count;
    }

    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_FetchHit)->Arg(64)->Arg(1024)->Arg(16384);

void BM_FetchStaleKey(benchmark::State& state) {
    constexpr std::size_t count = 64;
    saa::SlotMap<std::uint64_t> map(count);
    auto stale_keys = std::make_unique<saa::SlotMap<std::uint64_t>::Key[]>(count);
    for (std::size_t i = 0; i < count; ++i) {
        auto key = map.insert(static_cast<std::uint64_t>(i));
        if (!key) {
            state.SkipWithError("slot map filled before stale-key setup completed");
            return;
        }
        stale_keys[i] = *key;
    }
    for (std::size_t i = 0; i < count; ++i) {
        if (!map.erase(stale_keys[i]) || !map.insert(static_cast<std::uint64_t>(i))) {
            state.SkipWithError("could not prepare stale-key lookups");
            return;
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < count; ++i) {
            auto* value = map.fetch(stale_keys[i]);
            benchmark::DoNotOptimize(value);
        }
    }

    state.SetItemsProcessed(state.iterations() * static_cast<std::int64_t>(count));
}
BENCHMARK(BM_FetchStaleKey);

void BM_EraseAndReinsert(benchmark::State& state) {
    saa::SlotMap<std::uint64_t> map(1);
    auto key = map.insert(0);
    if (!key) {
        state.SkipWithError("could not prepare slot map");
        return;
    }
    std::uint64_t value = 1;
    for (auto _ : state) {
        if (!map.erase(*key)) {
            state.SkipWithError("erase failed during benchmark");
            break;
        }
        key = map.insert(value++);
        if (!key) {
            state.SkipWithError("insert failed during benchmark");
            break;
        }
        benchmark::DoNotOptimize(key);
    }

    state.SetItemsProcessed(state.iterations() * 2);
}
BENCHMARK(BM_EraseAndReinsert);

} // namespace

BENCHMARK_MAIN();
