#ifndef TOPA_EXECUTOR_HPP_
#define TOPA_EXECUTOR_HPP_

#ifdef TOPA_USE_TBB
#include <tbb/parallel_for_each.h>
#else
#include <algorithm>
#endif

namespace topa::detail {

template <typename It, typename Fn>
void execute(It begin, It end, Fn&& fn) {
#ifdef TOPA_USE_TBB
    tbb::parallel_for_each(begin, edn, std::forward<Fn>(fn));
#else
    std::for_each(begin, end, std::forward<Fn>(fn));
#endif
}

}  // namespace topa::detail

#endif
