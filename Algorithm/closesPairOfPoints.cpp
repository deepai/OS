#include <iostream>
#include <bits/stdc++.h>

using namespace std;

struct Solution {
	vector<vector<int>> &points;
	vector<vector<int>> pointsX;
	vector<vector<int>> pointsY;

	uint32_t getSquaredDist(uint32_t input) {
		return input * input;
	}

	uint32_t getDist(vector<int> &points_i, vector<int> &points_j) {
		uint32_t curr_dist = getSquaredDist(points_i[0] - points_j[0]) +
				getSquaredDist(points_i[1] - points_j[1]);

		return curr_dist;
	}

	uint32_t getDistance(int start, int end) {
		uint32_t dist = UINT32_MAX;
		for (int i = start; i < end - 1; i++)
			for (int j = i + 1; j < end; j++) {
				uint32_t curr_dist = getDist(pointsX[i], pointsX[j]);
				if (curr_dist < dist) {
					dist = curr_dist;
				}
			}

		return dist;
	}

	uint32_t getclosestDistance(int start, int end) {
		int num = end - start + 1;
		if (num <= 3) {
			return getDistance(start, end);
		}

		int mid = start + (end - start)/2;
		uint32_t distance = getclosestDistance(start, mid);
		distance = min(distance, getclosestDistance(mid + 1, end));

		vector<vector<int>> pointsforStrip;
		for (auto &each_point : pointsY) {
			//add points if they belong to the strip
			if(getSquaredDist(each_point[0] - pointsX[mid][0]) < distance) {
				pointsforStrip.push_back(each_point);
			}
		}

		for (int i = 0; i < pointsforStrip.size() - 1; i++) {
			for (int j = i + 1; j < pointsforStrip.size() && (getSquaredDist(pointsforStrip[j][1] - pointsforStrip[i][1]) < distance); j++) {
				uint32_t curr_distance = getDist(pointsforStrip[i], pointsforStrip[j]);
				if (curr_distance < distance) {
					distance = curr_distance;
				}
			}
		}

		return distance;
	}

	Solution(vector<vector<int>> &points) : points(points), pointsX(points), pointsY(points) {
		sort(pointsX.begin(), pointsX.end(), [](vector<int> &a, vector<int> &b) {
			return a[0] < b[0];
		});

		sort(pointsY.begin(), pointsY.end(), [](vector<int> &a, vector<int> &b) {
			return a[1] < b[1];
		});
	}

};

int main(int argc, char **argv) {
	vector<vector<int>> points = {{2, 3}, {12, 30}, {40, 50}, {5, 1}, {12, 10}};
	Solution sol(points);

	cout << sqrt(sol.getclosestDistance(0, points.size() - 1)) << endl;

	return 0;
}