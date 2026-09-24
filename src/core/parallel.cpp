#include "core/parallel.h"
#include <algorithm>
#include <thread>
#include <vector>

namespace pbe {

void parallel_for_rows(int rows, int threads, const std::function<void(int,int)>& fn) {
    if (rows <= 0) return;

    const int hw = static_cast<int>(std::thread::hardware_concurrency());
    const int desired = threads <= 0 ? (hw > 0 ? hw : 1) : threads;
    const int count = std::max(1, std::min(desired, rows));

    if (count == 1 || rows < 32) {
        fn(0, rows);
        return;
    }

    std::vector<std::thread> workers;
    workers.reserve(static_cast<size_t>(count));
    const int base = rows / count;
    const int remainder = rows % count;
    int begin = 0;

    for (int i = 0; i < count; ++i) {
        const int block = base + (i < remainder ? 1 : 0);
        const int end = begin + block;
        workers.emplace_back([begin, end, &fn]() { fn(begin, end); });
        begin = end;
    }

    for (auto& worker : workers) worker.join();
}

} // namespace pbe
