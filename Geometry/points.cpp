#include <iostream>

using namespace std;

struct Point {
	int x;
	int y;
};

int orientation(Point &p1, Point &p2, Point &p3)
{
	int val = (p3.y - p2.y) * (p2.x - p1.x) -  (p3.x - p2.x) * (p2.y - p1.y);
	if (val == 0) {
		return 0; //colinear
	} else {
		return ((val < 0) ? -1 : 1);
	}
}

// Given three collinear points p, q, r, the function checks if
// point q lies on line segment 'pr'
bool onSegment(Point p, Point q, Point r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return true;
  
    return false;
}

bool doIntersect(Point &p1, Point &p2, Point &p3, Point &p4) {
	//generic case
	int o1 = orientation(p1, p2, p3);
	int o2 = orientation(p1, p2, p4);
	int o3 = orientation(p3, p4, p1);
	int o4 = orientation(p3, p4, p2);

	if ((o1 != o2) && (o3 != o4)) {
		return true;
	}

	if (o1 == 0 && onSegment(p1, p3, p2)) return true;
	if (o2 == 0 && onSegment(p1, p4, p2)) return true;
	if (o3 == 0 && onSegment(p3, p1, p4)) return true;
	if (o4 == 0 && onSegment(p3, p2, p4)) return true;

	return false;
}

// Driver program to test above functions
int main()
{
    struct Point p1 = {1, 1}, q1 = {10, 1};
    struct Point p2 = {1, 2}, q2 = {10, 2};
  
    doIntersect(p1, q1, p2, q2)? cout << "Yes\n": cout << "No\n";
  
    p1 = {10, 0}, q1 = {0, 10};
    p2 = {0, 0}, q2 = {10, 10};
    doIntersect(p1, q1, p2, q2)? cout << "Yes\n": cout << "No\n";
  
    p1 = {-5, -5}, q1 = {0, 0};
    p2 = {1, 1}, q2 = {10, 10};
    doIntersect(p1, q1, p2, q2)? cout << "Yes\n": cout << "No\n";
  
    return 0;
}