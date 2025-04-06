# HPX Python API

This project demonstrates the integration of HPX (High Performance ParalleX) C++ library with Python using pybind11. It consists of three APIs, each showcasing different HPX capabilities exposed to Python.

## Setup

Before running any of the APIs, set up a Python virtual environment:

```bash
python3 -m venv venv
source venv/bin/activate
```

## API_1: Basic C++ Function Bindings

API_1 demonstrates exposing custom implemented C++ functions to Python using HPX. It includes:
- Matrix multiplication implementation in C++
- Factorial computation using HPX async
- Benchmarking to compare performance

### Build and Run API_1

```bash
cd API_1
rm -rf build
mkdir build
cd build
cmake -DHPX_WITH_MALLOC=system -DCMAKE_BUILD_TYPE=Release -Dpybind11_DIR=$(python -m pybind11 --cmakedir) -DHPX_DIR=$HOME/hpx-install/lib/cmake/HPX ..
make -j$(nproc)
export PYTHONPATH=$(pwd):$PYTHONPATH
cd ..
python3 test_script.py
python3 benchmark_1.py
```

## API_2: HPX Async and Reduce

API_2 demonstrates using HPX generics directly in Python including:
- HPX async for factorial calculation
- Parallel reduce operation for sum calculation
- Running Python functions through HPX's async mechanism
- Benchmarking performance

### Build and Run API_2

```bash
cd API_2
rm -rf build
mkdir build
cd build
cmake -DHPX_WITH_MALLOC=system -DCMAKE_BUILD_TYPE=Release -Dpybind11_DIR=$(python -m pybind11 --cmakedir) -DHPX_DIR=$HOME/hpx-install/lib/cmake/HPX ..
make -j$(nproc)
export PYTHONPATH=$(pwd):$PYTHONPATH
cd ..
python3 test_script.py
```

## API_3: HPX Parallel Sort

API_3 demonstrates HPX's parallel sort algorithm and benchmarks it against Python's built-in sort:
- HPX parallel sort implementation exposed to Python
- Comparison with Python's native sort
- Performance benchmarking with large datasets

### Build and Run API_3

```bash
cd API_3
rm -rf build
mkdir build
cd build
cmake -DHPX_WITH_MALLOC=system -DCMAKE_BUILD_TYPE=Release -Dpybind11_DIR=$(python -m pybind11 --cmakedir) -DHPX_DIR=$HOME/hpx-install/lib/cmake/HPX ..
make -j$(nproc)
export PYTHONPATH=$(pwd):$PYTHONPATH
cd ..
python3 test_script.py
python3 benchmark_3.py
```

## Technical Details

Each API demonstrates different aspects of integrating HPX with Python:

1. **API_1**: Basic approach to exposing C++ functions to Python with HPX runtime initialization.
2. **API_2**: Exposing HPX primitives like async and reduce directly to Python, showing how to handle the Python GIL.
3. **API_3**: Parallel algorithms (sort) implementation with performance benchmarking against Python equivalents.

All APIs handle HPX runtime initialization properly, ensuring the HPX backend works correctly with Python's threading model.