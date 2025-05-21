#pragma once

#ifdef TOPA_USE_TBB
#include <tbb/parallel_for_each.h>
#include <tbb/parallel_sort.h>
#else
#include <algorithm>
#endif

namespace topa::detail {

template <typename It, typename Fn>
void Execute(It begin, It end, Fn&& fn) {
#ifdef TOPA_USE_TBB
    tbb::parallel_for_each(begin, end, std::forward<Fn>(fn));
#else
    std::for_each(begin, end, std::forward<Fn>(fn));
#endif
}

template <typename It>
void Sort(It begin, It end) {
#ifdef TOPA_USE_TBB
    tbb::parallel_sort(begin, end);
#else
    std::sort(begin, end);
#endif
}

}  // namespace topa::detail
