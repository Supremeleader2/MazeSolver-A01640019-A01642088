// =======================================================================
// Brief description: Maze solver that finds a path from the entrance to 
// the exit of a boolean maze using two algorithmic techniques: Backtracking 
// and Branch and Bound.
//
// Authors: Angela Andrea Castro García A01640019
//          Andrea Villaseñor Jiménez A01642088
// Date: 01/09/2026
//
// INPUT  (standard input):
//     M N
//     M lines with N boolean values (0|1) separated by spaces
//     1 -> square can be walked on, 0 -> wall (cannot be crossed)
//     Start square is always (0,0), exit square is always (M-1,N-1)
//
// OUTPUT (standard output):
//     M x N solution matrix found with Backtracking, followed by
//     M x N solution matrix found with Branch and Bound.
//     If a maze has no solution, an all-zero matrix is printed.
//
// Movement is allowed in 4 directions and is explored in the fixed
// order: Down, Right, Up, Left.
//
// COMPLEXITY:
//     General Time Complexity : O(4^(M*N)) worst case for Backtracking.
//                               O(M*N * log(M*N)) for Branch and Bound.
//     General Space Complexity: O(M*N) for both techniques.
//
// Build: g++ -Wall -Wextra -std=c++17 -O2 main.cpp -o a.out
// Run  : ./a.out < in.txt
// =======================================================================

#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <climits>
#include <cstdlib>

using namespace std;

typedef vector<vector<int>> Matrix;

const int NUM_DIRECTIONS = 4;

// Direction vectors, explored in this fixed order: Down, Right, Up, Left.
const int DIRECTION_ROW[NUM_DIRECTIONS] = { 1, 0, -1, 0 };
const int DIRECTION_COL[NUM_DIRECTIONS] = { 0, 1, 0, -1 };

// -----------------------------------------------------------------------
// Purpose: Reads the maze dimensions and contents from standard input.
// Parameters:
//     rows - Reference to an integer to store the number of rows.
//     cols - Reference to an integer to store the number of columns.
//     maze - Reference to a Matrix to store the boolean grid.
// Return: void
// Time Complexity : O(M * N) where M is rows and N is columns.
// Space Complexity: O(M * N) to store the matrix.
// -----------------------------------------------------------------------
void readMaze(int &rows, int &cols, Matrix &maze) {
	if (!(cin >> rows >> cols)) {
		return;
	}
	maze.assign(rows, vector<int>(cols, 0));
	for (int row = 0; row < rows; ++row) {
		for (int col = 0; col < cols; ++col) {
			cin >> maze[row][col];
		}
	}
}

// -----------------------------------------------------------------------
// Purpose: Prints a boolean solution matrix to standard output.
// Parameters:
//     solution - Constant reference to the Matrix containing the path.
// Return: void
// Time Complexity : O(M * N) where M is rows and N is columns.
// Space Complexity: O(1) auxiliary space.
// -----------------------------------------------------------------------
void printSolution(const Matrix &solution) {
	for (size_t row = 0; row < solution.size(); ++row) {
		for (size_t col = 0; col < solution[row].size(); ++col) {
			cout << solution[row][col];
			if (col + 1 < solution[row].size()) {
				cout << ' ';
			}
		}
		cout << '\n';
	}
}

// =======================================================================
// BACKTRACKING TECHNIQUE
// =======================================================================

// -----------------------------------------------------------------------
// Purpose: Recursive utility function to explore paths using Backtracking.
// Parameters:
//     maze    - Constant reference to the original maze matrix.
//     path    - Reference to the matrix storing the current solution path.
//     visited - Reference to the matrix tracking visited cells.
//     row     - Integer representing the current row coordinate.
//     col     - Integer representing the current column coordinate.
//     rows    - Total number of rows in the maze.
//     cols    - Total number of columns in the maze.
// Return: boolean (true if a path to the exit is found, false otherwise).
// Time Complexity : O(4^(M*N)) in the worst case exploring all paths.
// Space Complexity: O(M * N) for the recursion stack and matrices.
// -----------------------------------------------------------------------
bool solveBacktrackingUtil(const Matrix &maze, Matrix &path, Matrix &visited, int row, int col, int rows, int cols) {
	if (row == rows - 1 && col == cols - 1 && maze[row][col] == 1) {
		path[row][col] = 1;
		return true;
	}

	bool isSafeCell = row >= 0 && row < rows && col >= 0 && col < cols &&
					   maze[row][col] == 1 && visited[row][col] == 0;
	bool pathFound = false;

	if (isSafeCell) {
		visited[row][col] = 1;
		path[row][col] = 1;

		for (int direction = 0; direction < NUM_DIRECTIONS && !pathFound; ++direction) {
			int nextRow = row + DIRECTION_ROW[direction];
			int nextCol = col + DIRECTION_COL[direction];
			if (solveBacktrackingUtil(maze, path, visited, nextRow, nextCol, rows, cols)) {
				pathFound = true;
			}
		}

		if (!pathFound) {
			path[row][col] = 0;
		}
	}

	return pathFound;
}

// -----------------------------------------------------------------------
// Purpose: Initializes structures and triggers the Backtracking algorithm.
// Parameters:
//     maze - Constant reference to the original maze matrix.
//     rows - Integer for total number of rows.
//     cols - Integer for total number of columns.
// Return: Matrix containing the solution path (or zeros if none).
// Time Complexity : O(4^(M*N)) worst case.
// Space Complexity: O(M * N) for visited and path matrices.
// -----------------------------------------------------------------------
Matrix solveWithBacktracking(const Matrix &maze, int rows, int cols) {
	Matrix path(rows, vector<int>(cols, 0));
	Matrix visited(rows, vector<int>(cols, 0));
	solveBacktrackingUtil(maze, path, visited, 0, 0, rows, cols);
	return path;
}

// =======================================================================
// BRANCH AND BOUND TECHNIQUE
// =======================================================================
struct Node {
	int row;
	int col;
	int g;
	int f;
	long order;
};

struct NodeComparator {
	bool operator()(const Node &left, const Node &right) const {
		if (left.f != right.f) {
			return left.f > right.f;
		}
		return left.order > right.order;
	}
};

// -----------------------------------------------------------------------
// Purpose: Calculates the Manhattan distance heuristic between two cells.
// Parameters:
//     row     - Current row coordinate.
//     col     - Current column coordinate.
//     goalRow - Target row coordinate.
//     goalCol - Target column coordinate.
// Return: Integer representing the absolute distance.
// Time Complexity : O(1) mathematical operation.
// Space Complexity: O(1) auxiliary space.
// -----------------------------------------------------------------------
int manhattanDistance(int row, int col, int goalRow, int goalCol) {
	return abs(goalRow - row) + abs(goalCol - col);
}

// -----------------------------------------------------------------------
// Purpose: Solves the maze using Branch and Bound (A* search algorithm).
// Parameters:
//     maze - Constant reference to the original maze matrix.
//     rows - Integer for total number of rows.
//     cols - Integer for total number of columns.
// Return: Matrix containing the optimal solution path.
// Time Complexity : O(M * N * log(M * N)) due to priority queue operations.
// Space Complexity: O(M * N) for matrices, priority queue, and parent tracking.
// -----------------------------------------------------------------------
Matrix solveWithBranchAndBound(const Matrix &maze, int rows, int cols) {
	Matrix path(rows, vector<int>(cols, 0));
	
	if (rows == 0 || cols == 0) {
		return path;
	}

	bool startAndExitOpen = maze[0][0] == 1 && maze[rows - 1][cols - 1] == 1;

	if (!startAndExitOpen) {
		return path;
	}

	const int INF = INT_MAX;
	vector<vector<int>> bestCost(rows, vector<int>(cols, INF));
	vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols, make_pair(-1, -1)));
	priority_queue<Node, vector<Node>, NodeComparator> pq;
	long orderCounter = 0;
	int goalRow = rows - 1;
	int goalCol = cols - 1;
	bool found = false;

	bestCost[0][0] = 0;
	pq.push(Node{ 0, 0, 0, manhattanDistance(0, 0, goalRow, goalCol), orderCounter++ });

	while (!pq.empty() && !found) {
		Node current = pq.top();
		pq.pop();
		
		bool isStaleEntry = current.g > bestCost[current.row][current.col];

		if (!isStaleEntry) {
			if (current.row == goalRow && current.col == goalCol) {
				found = true;
			}
			
			if (!found) {
				for (int direction = 0; direction < NUM_DIRECTIONS; ++direction) {
					int nextRow = current.row + DIRECTION_ROW[direction];
					int nextCol = current.col + DIRECTION_COL[direction];
					bool insideMaze = nextRow >= 0 && nextRow < rows && nextCol >= 0 && nextCol < cols;
					bool isOpenCell = insideMaze && maze[nextRow][nextCol] == 1;

					if (isOpenCell) {
						int newG = current.g + 1;
						
						if (newG < bestCost[nextRow][nextCol]) {
							bestCost[nextRow][nextCol] = newG;
							parent[nextRow][nextCol] = make_pair(current.row, current.col);
							int f = newG + manhattanDistance(nextRow, nextCol, goalRow, goalCol);
							pq.push(Node{ nextRow, nextCol, newG, f, orderCounter++ });
						}
					}
				}
			}
		}
	}

	if (found) {
		int row = goalRow;
		int col = goalCol;
		bool reachedOrigin = false;

		while (!reachedOrigin) {
			path[row][col] = 1;
			if (row == 0 && col == 0) {
				reachedOrigin = true;
			} else {
				pair<int, int> parentCell = parent[row][col];
				row = parentCell.first;
				col = parentCell.second;
			}
		}
	}

	return path;
}

// -----------------------------------------------------------------------
// Purpose: Program entry point. Coordinates input processing and outputs.
// Parameters: None
// Return: Integer 0 indicating successful execution.
// Time Complexity : O(General execution time of algorithms).
// Space Complexity: O(General memory footprint).
// -----------------------------------------------------------------------
int main() {
	int rows = 0, cols = 0;
	Matrix maze;

	readMaze(rows, cols, maze);

	if (rows == 0 || cols == 0) {
		return 0;
	}

	Matrix backtrackingSolution = solveWithBacktracking(maze, rows, cols);
	printSolution(backtrackingSolution);
	
	cout << "\n";

	Matrix branchAndBoundSolution = solveWithBranchAndBound(maze, rows, cols);
	printSolution(branchAndBoundSolution);

	return 0;
}

/*
Additional notes:
1. README Drafting: Claude AI (Anthropic) was used to structure, organize, and 
draft the general content of the README file, facilitating the synthesis of 
theoretical information regarding the Backtracking and Branch & Bound algorithms.
2. Code Documentation: The Gemini model (Google) was used to assist in detecting 
errors or inconsistencies in the provided coding standard, as well as for 
documenting each method/function in accordance with the standard's specifications.

References
Anthropic. (2026). Claude (Large language model) [Modelo de lenguaje grande]. 
    https://claude.ai

Google. (2026). Gemini (Large language model) [Modelo de lenguaje grande]. 
    https://gemini.google.com
*/