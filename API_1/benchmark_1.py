import hpx_bindings as hpx
import time
import random

##############################
# Benchmark: Factorial
##############################
n = 10
num_trials_factorial = 10000  # Adjust iterations for measurable timing

def python_factorial_manual(n):
    """Compute factorial using a simple for-loop."""
    result = 1
    for i in range(1, n + 1):
        result *= i
    return result

# HPX factorial benchmark
start_time = time.perf_counter()
for _ in range(num_trials_factorial):
    result_hpx_factorial = hpx.factorial(n)
hpx_factorial_time = time.perf_counter() - start_time

# Python factorial benchmark (using manual for-loop)
start_time = time.perf_counter()
for _ in range(num_trials_factorial):
    result_python_factorial = python_factorial_manual(n)
python_factorial_time = time.perf_counter() - start_time

# Verify that both results are identical
if result_hpx_factorial != result_python_factorial:
    print("Error: Factorial results do not match!")
else:
    print(f"Factorial results match: {result_hpx_factorial}")

print(f"HPX factorial({n}) over {num_trials_factorial} iterations took {hpx_factorial_time:.6f} seconds")
print(f"Python manual factorial({n}) over {num_trials_factorial} iterations took {python_factorial_time:.6f} seconds\n")

##############################
# Benchmark: Matrix Multiplication (Large Example)
##############################
# Generate larger matrices:
# Let A be 100 x 200 and B be 200 x 150. The resulting matrix will have 100*150 = 15,000 cells.
rows_A, cols_A = 100, 200
rows_B, cols_B = cols_A, 150

# For reproducibility, you can seed the random generator if desired.
random.seed(42)
A = [[random.randint(1, 10) for _ in range(cols_A)] for _ in range(rows_A)]
B = [[random.randint(1, 10) for _ in range(cols_B)] for _ in range(rows_B)]

def python_matrix_multiply(A, B):
    """
    Multiply two matrices A and B using nested loops.
    A is an m x n matrix and B is an n x p matrix.
    Returns the m x p result matrix.
    """
    m = len(A)
    n = len(A[0])
    p = len(B[0])
    # Initialize result matrix with zeros
    result = [[0 for _ in range(p)] for _ in range(m)]
    for i in range(m):
        for j in range(p):
            s = 0
            for k in range(n):
                s += A[i][k] * B[k][j]
            result[i][j] = s
    return result

# We use a lower iteration count here since each multiplication is more expensive.
num_trials_matrix = 10

# HPX matrix multiplication benchmark
start_time = time.perf_counter()
for _ in range(num_trials_matrix):
    result_hpx_matrix = hpx.matrix_multiply(A, B)
hpx_matrix_time = time.perf_counter() - start_time

# Python matrix multiplication benchmark using our function
start_time = time.perf_counter()
for _ in range(num_trials_matrix):
    result_python_matrix = python_matrix_multiply(A, B)
python_matrix_time = time.perf_counter() - start_time

# Validate that both implementations produce the same result
if result_hpx_matrix != result_python_matrix:
    print("Error: Matrix multiplication results do not match!")
else:
    print("Matrix multiplication results match.")

print(f"HPX matrix multiplication over {num_trials_matrix} iterations took {hpx_matrix_time:.6f} seconds")
print(f"Python matrix multiplication over {num_trials_matrix} iterations took {python_matrix_time:.6f} seconds")
