import hpx_bindings as hpx

# Factorial test
print("factorial(5) =", hpx.factorial(5))

# Matrix multiplication test
A = [[1, 2, 3],
     [4, 5, 6]]
B = [[7,  8],
     [9, 10],
     [11,12]]

R = hpx.matrix_multiply(A, B)
print("Matrix multiply result =", R)
# Expect [[58, 64], [139, 154]]
