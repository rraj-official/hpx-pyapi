import hpx_bindings as hpx

# Test the factorial function with input 5
print("factorial(5) =", hpx.factorial(5))

# Define two matrices for multiplication testing
A = [[1, 2, 3],
     [4, 5, 6]]
B = [[7,  8],
     [9, 10],
     [11,12]]

# Multiply matrices using the HPX parallel implementation
R = hpx.matrix_multiply(A, B)
print("Matrix multiply result =", R)
# Expected result: [[58, 64], [139, 154]]
