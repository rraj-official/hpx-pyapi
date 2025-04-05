import atexit
import hpx_bindings as hpx

# Start HPX runtime with 8 threads and register shutdown via atexit.
hpx.start_hpx(8)
atexit.register(hpx.stop_hpx)

print("Factorial(5) =", hpx.factorial(5))

def double(n):
    return n * 2
print("hpx_sync(double, 21) =", hpx.hpx_sync(double, 21))

numbers = list(range(1, 101))  # Sum numbers 1 to 100
print("Parallel reduce sum (1..100) =", hpx.parallel_reduce_sum(numbers))
