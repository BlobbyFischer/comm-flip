# Commutative Matrix Multiplication via Flip Graphs

This project contains C++ and Python code developed for a research paper on finding **commutative matrix multiplication schemes** using a **flip graph** approach.

---

## Repository Overview

### C++ Core

- `Rank1Tensor.cpp`, `Rank1Tensor.h`  
  Define the `Rank1Tensor` class representing individual rank-1 tensors that sum to form a full tensor.

- `Scheme.cpp`, `Scheme.h`  
  Define the `Scheme` class representing a full decomposition into rank-1 tensors. Implements the flip graph exploration logic.

- `main.cpp`  
  Entry point for the program. Takes an input tensor decomposition and performs the flip graph search.

- `Makefile`  
  Use `make` to compile the C++ code.

### Decompositions & Results

- `*.exp`  
  Files representing tensor decompositions. Each line is a rank-1 tensor. For example, `222.exp` is the standard `<2,2,2>` matrix multiplication tensor.

- `solutions/`  
  Contains output decompositions organized by tensor and rank.  
  Structure:
  ```
  solutions/
    example1/
      x8/  # Standard search results of rank 8
      y7/  # Marakov-like search results of rank 7
  ```

### Python Scripts

- `down.py`  
  Automates large-scale flip graph searches. Given a starting directory and prefix, it performs multiple attempts to lower the decomposition rank by traversing the flip graph.

---

## ⚙Build Instructions

Compile the C++ code with:

```bash
make
```

This creates the executable `flip`.

---

## Basic Usage

To run a single flip graph search:

```bash
./flip 222.exp 100000
```

- `222.exp`: Input decomposition (e.g., rank-8 standard `<2,2,2>` tensor).
- `100000`: Number of steps to explore in the flip graph.

Output:

```
k250411112.exp,7
```

This means a new file `k250411112.exp` was generated — a **rank 7** commutative decomposition of the `<2,2,2>` tensor.

---

## Advanced Search with `down.py`

Use `down.py` for larger, automated searches.

### Expected Directory Structure

- A folder under `solutions/` representing the target tensor (e.g., `example1`).
- Subfolders for each known rank with decompositions:
  ```
  solutions/example1/x8/
  solutions/example1/y7/
  ```
  - `x`, `y` = prefixes representing different starting strategies (e.g., standard, Marakov-like).
  - Each subfolder contains one or more `.exp` files to search from.

### Example Command

```bash
python3 down.py example1 x 10000 100 25 8
```

#### Arguments:
1. `example1`: Target tensor folder under `solutions/`
2. `x`: Prefix used for search
3. `10000`: Number of steps per instance
4. `100`: Max failed attempts before stopping
5. `25`: Number of successful reductions to find
6. `8`: Number of parallel instances

If successful, it will create:

```
solutions/example1/x7/
```

Containing 25 rank-7 commutative decompositions of `<2,2,2>`.

---

### More Examples

```bash
python3 down.py example2 x 1000000 100 25 8
python3 down.py example2 y 1000000 100 25 8
```

- `example2`: Refers to the `<3,3,3>` matrix multiplication tensor.
- `x`, `y`: Search strategies from different initial schemes.

---

## Notes

- The flip graph search is **stochastic** — results may vary between runs.
- Tensor decompositions are saved in `.exp` format for reuse and inspection.
- This project is research-oriented and assumes some familiarity with:
  - Tensor rank
  - Commutative decompositions
  - Matrix multiplication complexity

---

## Reference

This code was developed as part of research on exploring flip graph methods for reducing tensor rank in commutative matrix multiplication. For more details, see the accompanying paper:

**[Insert paper title/link here]**

---

## License

[MIT License or your preferred license here]
