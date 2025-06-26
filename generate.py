from sympy import Eq, Idx, IndexedBase, Symbol
from jinja2 import Environment, FileSystemLoader

env = Environment(loader=FileSystemLoader('templates'))

def genFunc(eqs, name="a_bc"):
    eqs = [eqs] if isinstance(eqs, Eq) else list(eqs)
    idx = next(s for eq in eqs for s in eq.free_symbols if isinstance(s, Idx))

    result = eqs[0].lhs.base
    inputs = sorted({b for eq in eqs for b in eq.atoms(IndexedBase) if b != result},
                    key=lambda b: b.name)

    array_syms = {sym for base in [result] + inputs for sym in base.free_symbols}
    scalars = sorted({s for eq in eqs for s in eq.free_symbols
                      if isinstance(s, Symbol) and s != idx and s not in array_syms},
                     key=lambda s: s.name)

    code = env.get_template("v4_parallel.hpp.jinja2").render(
        func_name=f"cpp__{name}", idx=idx,
        arrays=[result] + inputs, scalars=scalars, equations=eqs
    )
    with open(f"{name}.hpp", "w") as f:
        f.write(code)
    print(f"Generated {name}.hpp")

if __name__ == "__main__":
    i = Idx("i")
    a, b, c, r = map(IndexedBase, "a b c r".split())
    d = Symbol("d")
    genFunc(Eq(r[i], d*a[i] + b[i]*c[i]))
