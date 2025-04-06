#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <hpx/local/init.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/parallel_reduce.hpp> // Provides parallel reduction algorithms
#include <hpx/include/parallel_sort.hpp>   // Provides parallel sorting algorithms
#include <hpx/execution.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <thread>

// Namespace alias for pybind11
namespace py = pybind11;

// Global state variables to track HPX runtime status
static std::atomic<bool> hpx_running{false};
static std::mutex hpx_mutex;
static std::condition_variable hpx_cv;

// HPX main function that runs when the runtime initializes
int hpx_main(int argc, char* argv[])
{
    {
        std::lock_guard<std::mutex> lk(hpx_mutex);
        hpx_running = true;
    }
    hpx_cv.notify_all();
    // Return immediately as we only need to signal that runtime is ready
    return 0;
}

// Initialize the HPX runtime in a separate thread
void start_hpx(int threads)
{
    // Skip initialization if HPX is already running
    if (hpx_running)
    {
        std::cout << "HPX runtime is already running.\n";
        return;
    }

    // Configure HPX with the specified number of threads
    std::vector<std::string> cfg = {
        "hpx.os_threads=" + std::to_string(threads),
        "hpx.commandline.allow_unknown=1"
    };

    // Prepare fake command-line arguments for HPX initialization
    int argc = 1;
    char prog_name[] = "hpx_binding";
    char* my_argv[2];
    my_argv[0] = prog_name;
    my_argv[1] = nullptr;

    // Start HPX in a separate thread
    std::thread t([=]() {
        hpx::local::init_params init_args;
        init_args.cfg = cfg;
        // This blocks in this thread until HPX finishes
        hpx::local::init(hpx_main, 0, nullptr, init_args);
    });
    t.detach();

    // Wait for HPX to signal it's running
    {
        std::unique_lock<std::mutex> lk(hpx_mutex);
        // Wait with timeout to avoid hanging if initialization fails
        hpx_cv.wait_for(lk, std::chrono::seconds(10),
                        [] { return hpx_running.load(); });
        if (!hpx_running)
        {
            throw std::runtime_error("HPX initialization timed out");
        }
    }

    std::cout << "HPX runtime started with " << threads << " threads.\n";
}

// Placeholder for HPX shutdown (actual cleanup happens automatically)
void stop_hpx()
{
    // HPX cleanup happens automatically when the Python process ends
    std::cout << "Shutting down HPX runtime.\n";
    // Explicit stop can cause issues, so we rely on automatic cleanup
}

// Recursive factorial implementation using HPX async
std::uint64_t factorial(std::uint64_t n)
{
    if (n == 0)
        return 1;

    // Create an async task for the recursive call
    hpx::future<std::uint64_t> f = hpx::async([n]() {
        return factorial(n - 1);
    });

    return n * f.get();
}

// Python-friendly factorial wrapper that handles the GIL
std::uint64_t py_factorial(std::uint64_t n)
{
    // Release the Python GIL during HPX execution
    py::gil_scoped_release release;
    return factorial(n);
}

// Execute a Python function via HPX async
int hpx_sync(py::function f, int arg)
{
    py::gil_scoped_release release;
    auto fut = hpx::async([f, arg]() -> int {
        // Reacquire the GIL to call back into Python
        py::gil_scoped_acquire acquire;
        return f(arg).cast<int>();
    });
    return fut.get();
}

// Sum a vector of integers using HPX parallel reduce
int parallel_reduce_sum(const std::vector<int>& data)
{
    return hpx::reduce(hpx::execution::par, data.begin(), data.end(), 0);
}

// Sort a vector of integers using HPX parallel sort
std::vector<int> parallel_sort(const std::vector<int>& data)
{
    // Release the GIL during the sort operation
    py::gil_scoped_release release;

    // Create a copy of the input data
    std::vector<int> copy = data;

    // Perform parallel sort on the copy
    hpx::sort(hpx::execution::par, copy.begin(), copy.end());

    // Return the sorted vector
    return copy;
}

// Define the Python module and expose C++ functions
PYBIND11_MODULE(hpx_bindings, m)
{
    m.doc() = "HPX Python bindings exposing quickstart examples: factorial, parallel_reduce_sum, parallel_sort, etc.";

    // Expose runtime management functions
    m.def("start_hpx", &start_hpx, py::arg("threads") = 4,
          "Start the HPX runtime in a background thread (Program A style)");
    m.def("stop_hpx", &stop_hpx,
          "Dummy function that does nothing (HPX will stop automatically).");

    // Expose computational functions
    m.def("factorial", &py_factorial,
          "Compute factorial using HPX async recursion");
    m.def("hpx_sync", &hpx_sync,
          "Call a Python function via HPX async, synchronously blocking");
    m.def("parallel_reduce_sum", &parallel_reduce_sum,
          "Compute the sum of a list of integers using HPX reduce");

    // Expose parallel sort function
    m.def("parallel_sort", &parallel_sort,
          "Sort a list of integers using HPX parallel sort and return a new list");
}
