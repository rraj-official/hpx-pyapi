import atexit
import hpx_bindings as hpx

# Initialize HPX runtime with 8 threads and register shutdown function
hpx.start_hpx(8)
atexit.register(hpx.stop_hpx)

# Test the HPX factorial function with input 5
print("Factorial(5) =", hpx.factorial(5))

# Define a Python function to execute via HPX
def double(n):
    return n * 2
print("hpx_sync(double, 21) =", hpx.hpx_sync(double, 21))

# Test parallel reduction with a range of numbers 1-100
numbers = list(range(1, 101))  # Create a list with numbers 1 to 100
print("Parallel reduce sum (1..100) =", hpx.parallel_reduce_sum(numbers))
