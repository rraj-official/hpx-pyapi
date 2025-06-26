import numpy as np, symhpx

symhpx.hpx_init()

n = 100
va = np.arange(n, dtype=np.float64)
vb = np.arange(n, dtype=np.float64)
vc = np.arange(n, dtype=np.float64)
vr = np.empty(n, dtype=np.float64)

symhpx.cpp__a_bc(vr, va, vb, vc, 2.0)
print("vr[0:5] =", vr[:5])

symhpx.hpx_finalize()
