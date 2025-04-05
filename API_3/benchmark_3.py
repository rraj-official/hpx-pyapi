import atexit
import hpx_bindings as hpx
import time
import random

# Start HPX runtime with 8 threads
hpx.start_hpx(8)
atexit.register(hpx.stop_hpx)

# Generate a large list of random numbers for benchmarking
N = 10**7  # 10 million numbers
numbers = [random.randint(0, 10**7) for _ in range(N)]

# Benchmark Python's built-in sort
numbers_python = numbers.copy()
start_time = time.perf_counter()
sorted_python = sorted(numbers_python)
python_duration = time.perf_counter() - start_time

# Benchmark HPX parallel sort
numbers_hpx = numbers.copy()
start_time = time.perf_counter()
sorted_hpx = hpx.parallel_sort(numbers_hpx)
hpx_duration = time.perf_counter() - start_time

# Validate that both sorting methods yield the same result
if sorted_python != sorted_hpx:
    print("Error: Sorted lists do not match!")
else:
    print("Both sorting methods produce the same result.")

# Print benchmark results
print(f"Python sort took {python_duration:.6f} seconds")
print(f"HPX parallel sort took {hpx_duration:.6f} seconds")
