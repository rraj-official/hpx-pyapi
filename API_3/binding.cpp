#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <hpx/local/init.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/parallel_reduce.hpp> // Provides hpx::reduce
#include <hpx/include/parallel_sort.hpp>   // Provides hpx::sort
#include <hpx/execution.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

// For convenience
namespace py = pybind11;

// ------------------------------------------------------------------
// 1) Provide global flags & hpx_main that signals the HPX runtime
//    is up, same as in Program A.
// ------------------------------------------------------------------
static std::atomic<bool> hpx_running{false};
static std::mutex hpx_mutex;
static std::condition_variable hpx_cv;

int hpx_main(int argc, char* argv[])
{
    {
        std::lock_guard<std::mutex> lk(hpx_mutex);
        hpx_running = true;
    }
    hpx_cv.notify_all();
    // Return immediately, no real work. This mirrors Program A.
    return 0;
}

// ------------------------------------------------------------------
// 2) start_hpx: replicate the Program A approach of hpx::local::init
//    launched on a separate thread and immediately detached.
// ------------------------------------------------------------------
void start_hpx(int threads)
{
    // If it’s already running, do nothing.
    if (hpx_running)
    {
        std::cout << "HPX runtime is already running.\n";
        return;
    }

    // Build the HPX config (same as Program A).
    std::vector<std::string> cfg = {
        "hpx.os_threads=" + std::to_string(threads),
        "hpx.commandline.allow_unknown=1"
    };

    // Prepare fake command-line arguments for HPX init.
    int argc = 1;
    char prog_name[] = "hpx_binding";
    char* my_argv[2];
    my_argv[0] = prog_name;
    my_argv[1] = nullptr;

    // Launch init in a separate thread.
    std::thread t([=]() {
        hpx::local::init_params init_args;
        init_args.cfg = cfg;
        // This blocks *in this thread* until HPX finishes, but we detach below.
        hpx::local::init(hpx_main, 0, nullptr, init_args);
    });
    t.detach();

    // Wait for HPX to be marked “running,” just as in Program A.
    {
        std::unique_lock<std::mutex> lk(hpx_mutex);
        // Wait up to e.g. 10 seconds, to fail gracefully if HPX never starts.
        hpx_cv.wait_for(lk, std::chrono::seconds(10),
                        [] { return hpx_running.load(); });
        if (!hpx_running)
        {
            throw std::runtime_error("HPX initialization timed out");
        }
    }

    std::cout << "HPX runtime started with " << threads << " threads.\n";
}

// ------------------------------------------------------------------
// 3) stop_hpx: if you really want a function call, make it do nothing
//    or just log a message. Program A does not explicitly stop HPX.
//    If you do call hpx::local::stop(), it can hang if the HPX main
//    thread hasn’t exited. So either remove or leave a no-op.
// ------------------------------------------------------------------
void stop_hpx()
{
    // Program A never explicitly stops HPX; it cleans up when the
    // python process ends or after hpx_main returns. So let’s do
    // the same. If you want an explicit call, just do nothing here:
    std::cout << "Shutting down HPX runtime.\n";
    // If you *really* want to forcibly stop inside Python, you can try:
    // hpx::local::stop();
    // but be aware that can cause hangs unless hpx_main has returned.
}

// ------------------------------------------------------------------
// HPX-based routines
// ------------------------------------------------------------------
std::uint64_t factorial(std::uint64_t n)
{
    if (n == 0)
        return 1;

    // Recursively spawn tasks
    hpx::future<std::uint64_t> f = hpx::async([n]() {
        return factorial(n - 1);
    });

    return n * f.get();
}

std::uint64_t py_factorial(std::uint64_t n)
{
    // Release the GIL so HPX threads aren’t blocked by Python’s GIL
    py::gil_scoped_release release;
    return factorial(n);
}

int hpx_sync(py::function f, int arg)
{
    py::gil_scoped_release release;
    auto fut = hpx::async([f, arg]() -> int {
        // reacquire the GIL in the HPX thread
        py::gil_scoped_acquire acquire;
        return f(arg).cast<int>();
    });
    return fut.get();
}

int parallel_reduce_sum(const std::vector<int>& data)
{
    return hpx::reduce(hpx::execution::par, data.begin(), data.end(), 0);
}

// New parallel sort function
std::vector<int> parallel_sort(const std::vector<int>& data)
{
    // Release the GIL so HPX threads can proceed
    py::gil_scoped_release release;

    // Copy data (we do not want to mutate the input in-place)
    std::vector<int> copy = data;

    // Use HPX parallel sort
    hpx::sort(hpx::execution::par, copy.begin(), copy.end());

    // Return sorted copy
    return copy;
}

// ------------------------------------------------------------------
// Expose module using pybind11
// ------------------------------------------------------------------
PYBIND11_MODULE(hpx_bindings, m)
{
    m.doc() = "HPX Python bindings exposing quickstart examples: factorial, parallel_reduce_sum, parallel_sort, etc.";

    // Start/Stop calls
    m.def("start_hpx", &start_hpx, py::arg("threads") = 4,
          "Start the HPX runtime in a background thread (Program A style)");
    m.def("stop_hpx", &stop_hpx,
          "Dummy function that does nothing (HPX will stop automatically).");

    // Factorial, etc.
    m.def("factorial", &py_factorial,
          "Compute factorial using HPX async recursion");
    m.def("hpx_sync", &hpx_sync,
          "Call a Python function via HPX async, synchronously blocking");
    m.def("parallel_reduce_sum", &parallel_reduce_sum,
          "Compute the sum of a list of integers using HPX reduce");

    // New parallel sort binding
    m.def("parallel_sort", &parallel_sort,
          "Sort a list of integers using HPX parallel sort and return a new list");
}
