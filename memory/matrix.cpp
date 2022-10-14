#include <iostream>
#include <vector>

using namespace std;


// 1 2 3 4
// 12 13 14 5
// 11 16 15  6
// 10 9 8 7
                //right,, bottom, left, up
int dir[4][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

bool valid (int i, int j, int N) {
    return i >= 0 && j >= 0 && i <= N - 1 && j <= N - 1;
}

void fill(vector<vector<int>> &matrix, int N) {

    int curr_number = 1;
    int curr_dir = 0;

    int curr_y = 0, curr_x = 0;

    int num_steps = N - 1;
    int num_cells_visited = 1;

    matrix[curr_y][curr_x] = curr_number;

    while (num_cells_visited < N * N) {
        int last_cell = -1;
        for (int i = 0; i < num_steps; i++) {
            int new_y = curr_y + dir[curr_dir][0];
            int new_x = curr_x + dir[curr_dir][1];

            if (curr_dir == 3 && i == num_steps - 1) {
                continue;
            }

            matrix[new_y][new_x] = ++curr_number;

            curr_y = new_y;
            curr_x = new_x;
        }

        // curr_y -= dir[curr_dir][0];
        // curr_x -= dir[curr_dir][1];

        if (curr_dir == 3) {
            num_steps -= 1;
        }

        curr_dir = (curr_dir + 1)%4;

        num_cells_visited++;
    }
}

int main() {
    int N = 5;
    vector<vector<int>> matrix(N, vector<int>(N, 0));
    fill(matrix, N);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            cout << matrix[i][j] << " "; 
        }
        cout << endl;
    }

    return 0;
}




// __shared_ array[2];

// //warp size = 32
// //CTA 64


// _sum(&input, &output, int N) {
//     //gid =   64            N/64       [0...N/64 - 1]
//     int gid = threadidx.x + blockDim.x * blockidx.x;
//     int in_data;
//     int local_wg;

//     if (gid < N) {
//         in_data = input[gid];
//         //atomic operation

//         //local_wg = threadIdx.x % 32;

//         array[0] += in_data;
//     }

//     __syncthreads();

//     //atomic sum
//     if (threadidx.x == 0)
//         output += array[0];
// }