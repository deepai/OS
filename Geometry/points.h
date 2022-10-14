#pragma once

template<typename T>
struct point2d {
    T x, y;
    point2d() {}
    point2d(T x, T y): x(x), y(y) {}
    point2d& operator+=(const point2d &t) {
        x += t.x;
        y += t.y;
        return *this;
    }
    point2d& operator-=(const point2d &t) {
        x -= t.x;
        y -= t.y;
        return *this;
    }
    point2d& operator*=(T t) {
        x *= t;
        y *= t;
        return *this;
    }
    point2d& operator/=(T t) {
        x /= t;
        y /= t;
        return *this;
    }
    point2d operator+(const point2d &t) const {
        return point2d(*this) += t;
    }
    point2d operator-(const point2d &t) const {
        return point2d(*this) -= t;
    }
    point2d operator*(T t) const {
        return point2d(*this) *= t;
    }
    point2d operator/(T t) const {
        return point2d(*this) /= t;
    }
};

template<typename T>
point2d operator*(T a, point2d<T> b) {
    return b * a;
}

template<typename T>
T dot(point2d<T> a, point2d<T> b) {
    return a.x * b.x + a.y * b.y;
}

template<typename T>
struct point3d {
    T x, y, z;
    point3d() {}
    point3d(T x, T y, T z): x(x), y(y), z(z) {}
    point3d& operator+=(const point3d &t) {
        x += t.x;
        y += t.y;
        z += t.z;
        return *this;
    }
    point3d& operator-=(const point3d &t) {
        x -= t.x;
        y -= t.y;
        z -= t.z;
        return *this;
    }
    point3d& operator*=(T t) {
        x *= t;
        y *= t;
        z *= t;
        return *this;
    }
    point3d& operator/=(T t) {
        x /= t;
        y /= t;
        z /= t;
        return *this;
    }
    point3d operator+(const point3d &t) const {
        return point3d(*this) += t;
    }
    point3d operator-(const point3d &t) const {
        return point3d(*this) -= t;
    }
    point3d operator*(T t) const {
        return point3d(*this) *= t;
    }
    point3d operator/(T t) const {
        return point3d(*this) /= t;
    }
};

template<typename T>
point3d operator*(T a, point3d<T> b) {
    return b * a;
}

template<typename T>
T dot(point3d<T> a, point3d<T> b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T, typename Z>
T norm(Z a) {
    return dot(a, a);
}

template<typename Z>
double abs(Z a) {
    return sqrt(norm(a));
}

template<typename Z>
double proj(Z a, Z b) {
    return dot(a, b) / abs(b);
}

template<typename Z>
double angle(Z a, Z b) {
    return acos(dot(a, b) / abs(a) / abs(b));
}