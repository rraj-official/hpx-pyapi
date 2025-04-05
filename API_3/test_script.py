import atexit
import hpx_bindings as hpx

# Start HPX runtime
hpx.start_hpx(8)
atexit.register(hpx.stop_hpx)

# Example usage of the new parallel_sort function
numbers = [5, 3, 4, 1, 2]
sorted_numbers = hpx.parallel_sort(numbers)
print("Unsorted numbers =", numbers)
print("Sorted numbers =", sorted_numbers)
