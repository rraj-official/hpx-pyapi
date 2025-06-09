import atexit
import hpx_bindings as hpx
import time
import random

# Initialize HPX runtime with 8 threads and register cleanup function
hpx.start_hpx(4)
atexit.register(hpx.stop_hpx)

# Generate a large dataset of random integers for sorting benchmark
N = 10**7  # 10 million numbers
numbers = [random.randint(0, 10**8) for _ in range(N)]

# Benchmark Python's built-in sort function
numbers_python = numbers.copy()
start_time = time.perf_counter()
sorted_python = sorted(numbers_python)
python_duration = time.perf_counter() - start_time

# Benchmark HPX parallel sort implementation
numbers_hpx = numbers.copy()
start_time = time.perf_counter()
sorted_hpx = hpx.parallel_sort(numbers_hpx)
hpx_duration = time.perf_counter() - start_time

# Verify both sorting algorithms produce identical results
if sorted_python != sorted_hpx:
    print("Error: Sorted lists do not match!")
else:
    print("Both sorting methods produce the same result.")

# Display performance comparison results
print(f"Python sort took {python_duration:.6f} seconds")
print(f"HPX parallel sort took {hpx_duration:.6f} seconds")
