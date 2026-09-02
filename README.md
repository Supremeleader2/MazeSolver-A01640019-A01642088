# Maze Solver — Backtracking & Branch and Bound

## Authors
- Angela Andrea Castro García — A01640019
- Andrea Villaseñor Jiménez — A01642088

## Description
This program solves a boolean maze using two different algorithmic
techniques:

1. **Backtracking** — an exhaustive recursive search that tries every
   possible move (in the fixed order Down, Right, Up, Left) and backs up
   whenever it hits a dead end, until it finds a path from the entrance to
   the exit.
2. **Branch and Bound** — implemented as an A* search that uses the
   Manhattan distance to the goal as a heuristic, expanding the most
   promising node first via a priority queue, guaranteeing a shortest path.

The maze is always entered at cell `(0,0)` and exited at cell `(M-1, N-1)`.
A cell with value `1` can be walked on; a cell with value `0` is a wall.

## Input Format
```
M N
M lines, each with N boolean values (0|1) separated by spaces
```

## Output Format
Two M x N matrices printed to standard output:
1. The solution path found with **Backtracking**.
2. A blank line separator.
3. The solution path found with **Branch and Bound**.

If the maze has no solution, an all-zero matrix is printed for that
technique.

## Computational Complexity

| Technique          | Time Complexity          | Space Complexity |
|---------------------|---------------------------|-------------------|
| Backtracking         | O(4^(M·N)) worst case    | O(M·N)            |
| Branch and Bound (A*)| O(M·N · log(M·N))        | O(M·N)            |

- **Backtracking**: in the worst case, every cell can branch into up to 4
  directions, so the recursion tree can grow exponentially with the number
  of cells explored before a valid path (or full exhaustion) is found.
- **Branch and Bound**: each cell is inserted into the priority queue at
  most a constant number of times, and each priority queue operation costs
  O(log(M·N)), giving an overall O(M·N · log(M·N)) bound.
- Both techniques use O(M·N) auxiliary space for the maze, visited/cost
  matrices, and the recursion stack or priority queue.

## Build
```bash
g++ -Wall -Wextra -std=c++17 -O2 main.cpp -o main
```

## Run

### Single test case
```bash
./main < test1.txt
```
Here, `test1.txt` is the name of the input file being redirected into the
program via `<`. Replace it with the name of whichever test case file you
want to run (e.g. `test2.txt`, `test3.txt`, etc.).

### Run all test cases at once
To run every `.txt` test file in the current folder and print each one's
output with a header identifying which test it belongs to, use:

```bash
for archivo in *.txt; do echo "=== $archivo ==="; ./main < "$archivo"; echo ""; done
```

This command loops through all `test*.txt` files, prints a header
(`=== test1.txt ===`, `=== test2.txt ===`, etc.) before each run so the
output of every test case is clearly identified in the terminal, executes
`main` with that file redirected as standard input, and adds a blank line
after each result for readability.

## Project Structure
```
StudentNumber_Act1.3/
├── main.cpp        # Source code (Backtracking + Branch and Bound)
├── README.md        # This file
├── test1.txt ... test13.txt   # Test cases
```

## Test Cases
The `test*.txt` files cover different scenarios for both techniques,
including:
- A maze with a valid, straightforward path.
- A maze with no possible solution (all-zero output expected).
- A maze where the entrance or exit itself is blocked (`0`).
- Larger/extreme mazes to validate efficiency and correctness under
  bigger inputs.

## AI Tool Usage Disclosure
- **README drafting**: Claude AI (Anthropic) was used to structure,
  organize, and draft the general content of this README file, including
  the theoretical summary of the Backtracking and Branch and Bound
  algorithms.
- **Code documentation**: The Gemini model (Google) was used to help
  detect errors/inconsistencies against the required coding standard and
  to document each method/function according to that standard.