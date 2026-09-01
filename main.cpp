// =======================================================================
// Maze solver that finds a path from the entrance to the exit of a
// boolean maze using two algorithmic techniques: Backtracking and
// Branch and Bound.
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
// Build: g++ -Wall -Wextra -std=c++17 -O2 main.cpp -o main
// Run  : ./main
// =======================================================================

#include <iostream>
#include <vector>
#include <queue>
#include <utility>
#include <climits>
#include <cstdlib>
#include <sstream>

using namespace std;

typedef vector<vector<int>> Matrix;

const int NUM_DIRECTIONS = 4;

// Direction vectors, explored in this fixed order: Down, Right, Up, Left.
const int DIRECTION_ROW[NUM_DIRECTIONS] = { 1, 0, -1, 0 };
const int DIRECTION_COL[NUM_DIRECTIONS] = { 0, 1, 0, -1 };

// -----------------------------------------------------------------------
// Reads the maze dimensions and contents from standard input.
// -----------------------------------------------------------------------
void readMaze(int &rows, int &cols, Matrix &maze) {
    cin >> rows >> cols;
    maze.assign(rows, vector<int>(cols, 0));
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            cin >> maze[row][col];
        }
    }
}

// -----------------------------------------------------------------------
// Reads the maze from a string (for testing)
// -----------------------------------------------------------------------
void readMazeFromString(const string& input, int &rows, int &cols, Matrix &maze) {
    istringstream iss(input);
    iss >> rows >> cols;
    
    maze.assign(rows, vector<int>(cols, 0));
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            iss >> maze[row][col];
        }
    }
}

// -----------------------------------------------------------------------
// Prints a boolean solution matrix to standard output.
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

int manhattanDistance(int row, int col, int goalRow, int goalCol) {
    return abs(goalRow - row) + abs(goalCol - col);
}

Matrix solveWithBranchAndBound(const Matrix &maze, int rows, int cols) {
    Matrix path(rows, vector<int>(cols, 0));
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

        if (isStaleEntry) {
            continue;
        }
        
        if (current.row == goalRow && current.col == goalCol) {
            found = true;
            break;
        }
        
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

// =======================================================================
// TEST FUNCTIONS
// =======================================================================

void runTest(const string& testName, const string& input) {
    cout << "\n" << string(60, '=') << "\n";
    cout << "TEST: " << testName << "\n";
    cout << string(60, '=') << "\n";
    
    int rows, cols;
    Matrix maze;
    readMazeFromString(input, rows, cols, maze);
    
    cout << "\nOriginal Maze:\n";
    printSolution(maze);
    cout << "\n";

    Matrix backtrackingSolution = solveWithBacktracking(maze, rows, cols);
    cout << "Backtracking Solution:\n";
    printSolution(backtrackingSolution);
    cout << "\n";

    Matrix branchAndBoundSolution = solveWithBranchAndBound(maze, rows, cols);
    cout << "Branch and Bound Solution:\n";
    printSolution(branchAndBoundSolution);
    cout << "\n";
}

// -----------------------------------------------------------------------
// Program entry point - runs all tests
// -----------------------------------------------------------------------
int main() {
    cout << "========================================\n";
    cout << "MAZE SOLVER - Backtracking vs Branch & Bound\n";
    cout << "========================================\n";
    cout << "Running Test Suite...\n\n";

    // Test 1: Simple 3x3 open maze (has solution)
    string test1 = R"(3 3
1 1 1
1 1 1
1 1 1)";
    runTest("Simple 3x3 Open Maze", test1);

    // Test 2: 3x3 with wall (has solution)
    string test2 = R"(3 3
1 1 1
1 0 1
1 1 1)";
    runTest("3x3 Maze with Wall", test2);

    // Test 3: 3x3 with no solution
    string test3 = R"(3 3
1 0 1
1 0 1
1 1 0)";
    runTest("3x3 Maze - No Solution", test3);

    // Test 4: 4x4 maze with obstacles (has solution)
    string test4 = R"(4 4
1 1 1 1
1 0 0 1
1 1 1 1
1 1 1 1)";
    runTest("4x4 Maze with Obstacles", test4);

    // Test 5: 1x1 maze (start = exit)
    string test5 = R"(1 1
1)";
    runTest("1x1 Maze - Start equals Exit", test5);

    // Test 6: 2x2 maze
    string test6 = R"(2 2
1 1
1 1)";
    runTest("2x2 Open Maze", test6);

    // Test 7: Start blocked (no solution)
    string test7 = R"(3 3
0 1 1
1 1 1
1 1 1)";
    runTest("Start Blocked - No Solution", test7);

    // Test 8: Exit blocked (no solution)
    string test8 = R"(3 3
1 1 1
1 1 1
1 1 0)";
    runTest("Exit Blocked - No Solution", test8);

    // Test 9: 5x5 maze with path
    string test9 = R"(5 5
1 1 1 1 1
1 0 0 0 1
1 1 1 0 1
0 0 1 0 1
1 1 1 1 1)";
    runTest("5x5 Complex Maze", test9);

    // Test 10: Large open maze (8x8)
    string test10 = R"(8 8
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1)";
    runTest("8x8 Open Maze (Large)", test10);

    cout << "\n" << string(60, '=') << "\n";
    cout << "ALL TESTS COMPLETED\n";
    cout << string(60, '=') << "\n";

    return 0;
}