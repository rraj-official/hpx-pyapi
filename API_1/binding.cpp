#include <pybind11/pybind11.h>
#include <pybind11/stl.h>          // for automatic conversion between STL and Python types
#include <hpx/hpx_init.hpp>
#include <hpx/include/async.hpp>
#include <hpx/include/runtime.hpp>
#include <hpx/include/lcos.hpp>      // for hpx::wait_all
#include <hpx/execution.hpp>

#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <atomic>

// Global state
static std::atomic<bool> hpx_running{false};
static std::mutex hpx_mutex;
static std::condition_variable hpx_cv;

// Simple HPX main function that will run when HPX starts
int hpx_main(int argc, char* argv[])
{
    {
        std::lock_guard<std::mutex> lk(hpx_mutex);
        hpx_running = true;
    }
    hpx_cv.notify_all();
    return 0;
}

// Initialize HPX
void init_hpx(int threads)
{
    if (hpx_running)
        return;
        
    // Start HPX in a separate thread
    std::thread t([threads]() {
        std::vector<std::string> cfg = {
            "hpx.os_threads=" + std::to_string(threads),
            "hpx.commandline.allow_unknown=1"
        };
        
        // Create dummy args for hpx::init
        int argc = 1;
        char prog_name[] = "hpx_binding";
        char* argv[] = {prog_name, nullptr};
            
        hpx::init_params init_args;
        init_args.cfg = cfg;
        
        hpx::init(hpx_main, argc, argv, init_args);
    });
    t.detach();
    
    // Wait for HPX to start
    {
        std::unique_lock<std::mutex> lk(hpx_mutex);
        if (!hpx_running) {
            hpx_cv.wait_for(lk, std::chrono::seconds(10), [] { return hpx_running.load(); });
        }
        if (!hpx_running) {
            throw std::runtime_error("HPX initialization timed out");
        }
    }
}

// Helper function to compute product of numbers in the range [start, end].
// For small ranges (size <= threshold), the product is computed sequentially.
// Otherwise, the range is split into two halves, each computed asynchronously.
std::uint64_t parallel_factorial_range(std::uint64_t start, std::uint64_t end)
{
    constexpr std::uint64_t threshold = 10; // adjust as needed
    if (end - start + 1 <= threshold)
    {
         std::uint64_t result = 1;
         for (std::uint64_t i = start; i <= end; ++i)
             result *= i;
         return result;
    }
    else
    {
         std::uint64_t mid = start + (end - start) / 2;
         auto f1 = hpx::async([start, mid]() {
             return parallel_factorial_range(start, mid);
         });
         auto f2 = hpx::async([mid, end]() {
             return parallel_factorial_range(mid + 1, end);
         });
         return f1.get() * f2.get();
    }
}

// Factorial implementation using parallel asynchronous tasks.
std::uint64_t factorial(std::uint64_t n)
{
    if(n == 0)
        return 1;
    return parallel_factorial_range(1, n);
}

// Matrix multiplication using HPX asynchronous tasks with a threshold for small workloads.
std::vector<std::vector<int>> matrix_multiply(
    const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B)
{
    if (A.empty() || B.empty())
        return {};
        
    size_t rowsA = A.size();
    size_t colsA = A[0].size();
    size_t rowsB = B.size();
    size_t colsB = B[0].size();
    
    if (colsA != rowsB)
        throw std::runtime_error("Matrix dimension mismatch");
    
    // Initialize result matrix.
    std::vector<std::vector<int>> R(rowsA, std::vector<int>(colsB, 0));

    // If the workload is small, perform sequential multiplication.
    constexpr size_t async_threshold = 1000; // threshold for total cells in R
    if (rowsA * colsB < async_threshold)
    {
        for (size_t i = 0; i < rowsA; i++) {
            for (size_t j = 0; j < colsB; j++) {
                int sum = 0;
                for (size_t k = 0; k < colsA; k++) {
                    sum += A[i][k] * B[k][j];
                }
                R[i][j] = sum;
            }
        }
        return R;
    }
    
    // For larger matrices, use asynchronous tasks to compute each row.
    std::vector<hpx::future<void>> futures;
    futures.reserve(rowsA);
    
    for (std::size_t i = 0; i < rowsA; i++) {
        futures.push_back(hpx::async([&, i]() {
            for (std::size_t j = 0; j < colsB; j++) {
                int sum = 0;
                for (std::size_t k = 0; k < colsA; k++) {
                    sum += A[i][k] * B[k][j];
                }
                R[i][j] = sum;
            }
        }));
    }
    
    hpx::wait_all(futures.begin(), futures.end());
    return R;
}

// Python wrapper functions
std::uint64_t py_factorial(std::uint64_t n)
{
    return factorial(n);
}

std::vector<std::vector<int>> py_matrix_multiply(
    const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B)
{
    return matrix_multiply(A, B);
}

namespace py = pybind11;

PYBIND11_MODULE(hpx_bindings, m)
{
    m.doc() = "Minimal HPX python bindings for factorial and matrix multiplication";
    
    // Set up number of threads
    int threads = std::thread::hardware_concurrency();
    if (const char* env_p = std::getenv("HPX_NUM_THREADS")) {
        int t = std::atoi(env_p);
        if (t > 0) threads = t;
    }
    
    // Initialize HPX runtime
    try {
        init_hpx(threads);
        std::cout << "HPX runtime started with " << threads << " threads" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error starting HPX runtime: " << e.what() << std::endl;
    }
    
    // Expose functions to Python
    m.def("factorial", &py_factorial, py::arg("n"),
          "Compute factorial(n) using HPX asynchronous tasks");
    m.def("matrix_multiply", &py_matrix_multiply, py::arg("A"), py::arg("B"),
          "Multiply two matrices in parallel using HPX asynchronous tasks (or sequentially for small matrices)");
}
