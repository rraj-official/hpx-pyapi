#pragma once
#include <cassert>
#include <vector>
#include <hpx/parallel/algorithms/for_loop.hpp>
#include <hpx/execution.hpp>

inline void cpp__a_bc(
    std::vector<double>& r,
const std::vector<double>& a,
const std::vector<double>& b,
const std::vector<double>& c,
const double& d
)
{
    const int n = a.size();
    assert(n == r.size());

    /* HPX ≥ 1.8: use experimental::for_loop */
    hpx::experimental::for_loop(
        hpx::execution::par, 0, n,
        [&](int i) {
r[i] = d*a[i] + b[i]*c[i];
}
    );
}