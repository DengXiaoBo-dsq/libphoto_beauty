#pragma once
#include <functional>
namespace pbe {
void parallel_for_rows(int rows, int threads, const std::function<void(int,int)>& fn);
}
