import atexit
import hpx_bindings as hpx

# Initialize HPX runtime with 8 threads and register cleanup
hpx.start_hpx(8)
atexit.register(hpx.stop_hpx)

# Test the HPX parallel sort function with a simple list
numbers = [5, 3, 4, 1, 2]
sorted_numbers = hpx.parallel_sort(numbers)
print("Unsorted numbers =", numbers)
print("Sorted numbers =", sorted_numbers)
