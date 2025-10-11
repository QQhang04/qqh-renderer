#pragma once
#include <cassert>
#include <iostream>

namespace math {

    template<typename T>
    class Vector3;

    template<typename T>
    class Vector4;

    template<typename T>
    class Vector2 {
    public:
        // ===== ���캯�� =====
        Vector2() : x(0), y(0) {}
        Vector2(T x, T y) : x(x), y(y) {}
        Vector2(const Vector2<T>& v) : x(v.x), y(v.y) {}
        Vector2(const Vector3<T>& v) : x(v.x), y(v.y) {}
        Vector2(const Vector4<T>& v) : x(v.x), y(v.y) {}

        // ===== �������� =====
        T operator[](int i) const {
            assert(i >= 0 && i < 2);
            return (i == 0) ? x : y;
        }

        T& operator[](int i) {
            assert(i >= 0 && i < 2);
            return (i == 0) ? x : y;
        }

        // ===== ��ֵ����� =====
        Vector2<T>& operator=(const Vector2<T>& v) {
            x = v.x; y = v.y;
            return *this;
        }

        Vector2<T>& operator=(const Vector3<T>& v) {
            x = v.x; y = v.y;
            return *this;
        }

        Vector2<T>& operator=(const Vector4<T>& v) {
            x = v.x; y = v.y;
            return *this;
        }

        // ===== �����ӷ� =====
        Vector2<T> operator+(const Vector2<T>& v) const {
            return Vector2<T>(x + v.x, y + v.y);
        }

        Vector2<T>& operator+=(const Vector2<T>& v) {
            x += v.x; y += v.y;
            return *this;
        }

        // ===== �������� =====
        Vector2<T> operator-(const Vector2<T>& v) const {
            return Vector2<T>(x - v.x, y - v.y);
        }

        Vector2<T>& operator-=(const Vector2<T>& v) {
            x -= v.x; y -= v.y;
            return *this;
        }

        // ===== ȡ���� =====
        Vector2<T> operator-() const {
            return Vector2<T>(-x, -y);
        }

        // ===== �����˷� =====
        Vector2<T> operator*(T s) const {
            return Vector2<T>(x * s, y * s);
        }

        Vector2<T>& operator*=(T s) {
            x *= s; y *= s;
            return *this;
        }

        // ===== �����˷� =====
        Vector2<T> operator*(Vector2<T> s) const {
            return Vector2<T>(x * s.x, y * s.y);
        }

        // ===== �������� =====
        Vector2<T> operator/(T s) const {
            assert(s != 0);
            float inv = static_cast<T>(1) / s;
            return Vector2<T>(x * inv, y * inv);
        }

        Vector2<T>& operator/=(T s) {
            assert(s != 0);
            float inv = static_cast<T>(1) / s;
            x *= inv; y *= inv;
            return *this;
        }

        // ===== �Ƚ� =====
        bool operator==(const Vector2<T>& v) const {
            return x == v.x && y == v.y;
        }

        bool operator!=(const Vector2<T>& v) const {
            return !(*this == v);
        }

        // ===== ��ӡ =====
        void print() const {
            std::cout << "Vector2(" << x << ", " << y << ")" << std::endl;
        }

    public:
        T x, y;
    };

    template<typename T>
    class Vector3 {
    public:
        // ===== ���캯�� =====
        Vector3() : x(0), y(0), z(0) {}
        Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
        Vector3(const Vector3<T>& v) : x(v.x), y(v.y), z(v.z) {}
        Vector3(const Vector2<T>& v, T z = 0) : x(v.x), y(v.y), z(z) {}
        Vector3(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z) {}

        // ===== �������� =====
        T operator[](int i) const {
            assert(i >= 0 && i < 3);
            return (i == 0) ? x : (i == 1 ? y : z);
        }

        T& operator[](int i) {
            assert(i >= 0 && i < 3);
            return (i == 0) ? x : (i == 1 ? y : z);
        }

        // ===== ��ֵ����� =====
        Vector3<T>& operator=(const Vector3<T>& v) {
            x = v.x; y = v.y; z = v.z;
            return *this;
        }

        Vector3<T>& operator=(const Vector2<T>& v) {
            x = v.x; y = v.y; z = 0;
            return *this;
        }

        Vector3<T>& operator=(const Vector4<T>& v) {
            x = v.x; y = v.y; z = v.z;
            return *this;
        }

        // ===== �����ӷ� =====
        Vector3<T> operator+(const Vector3<T>& v) const {
            return Vector3<T>(x + v.x, y + v.y, z + v.z);
        }

        Vector3<T>& operator+=(const Vector3<T>& v) {
            x += v.x; y += v.y; z += v.z;
            return *this;
        }

        // ===== �������� =====
        Vector3<T> operator-(const Vector3<T>& v) const {
            return Vector3<T>(x - v.x, y - v.y, z - v.z);
        }

        Vector3<T>& operator-=(const Vector3<T>& v) {
            x -= v.x; y -= v.y; z -= v.z;
            return *this;
        }

        // ===== ȡ���� =====
        Vector3<T> operator-() const {
            return Vector3<T>(-x, -y, -z);
        }

        // ===== �����˷� =====
        Vector3<T> operator*(T s) const {
            return Vector3<T>(x * s, y * s, z * s);
        }

        Vector3<T>& operator*=(T s) {
            x *= s; y *= s; z *= s;
            return *this;
        }

        // ===== �������� =====
        Vector3<T> operator/(T s) const {
            assert(s != 0);
            T inv = static_cast<T>(1) / s;
            return Vector3<T>(x * inv, y * inv, z * inv);
        }

        Vector3<T>& operator/=(T s) {
            assert(s != 0);
            T inv = static_cast<T>(1) / s;
            x *= inv; y *= inv; z *= inv;
            return *this;
        }

        // ===== �Ƚ� =====
        bool operator==(const Vector3<T>& v) const {
            return x == v.x && y == v.y && z == v.z;
        }

        bool operator!=(const Vector3<T>& v) const {
            return !(*this == v);
        }

        // ===== ��ӡ =====
        void print() const {
            std::cout << "Vector3(" << x << ", " << y << ", " << z << ")" << std::endl;
        }

    public:
        T x, y, z;
    };

    template<typename T>
    class Vector4 {
    public:
        // ===== ���캯�� =====
        Vector4() : x(0), y(0), z(0), w(0) {}
        Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
        Vector4(const Vector4<T>& v) : x(v.x), y(v.y), z(v.z), w(v.w) {}
        Vector4(const Vector2<T>& v, T z = 0, T w = 0) : x(v.x), y(v.y), z(z), w(w) {}
        Vector4(const Vector3<T>& v, T w = 0) : x(v.x), y(v.y), z(v.z), w(w) {}

        // ===== �������� =====
        T operator[](int i) const {
            assert(i >= 0 && i < 4);
            switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
            }
        }

        T& operator[](int i) {
            assert(i >= 0 && i < 4);
            switch (i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
            }
        }

        // ===== ��ֵ����� =====
        Vector4<T>& operator=(const Vector4<T>& v) {
            x = v.x; y = v.y; z = v.z; w = v.w;
            return *this;
        }

        Vector4<T>& operator=(const Vector3<T>& v) {
            x = v.x; y = v.y; z = v.z; w = 0;
            return *this;
        }

        Vector4<T>& operator=(const Vector2<T>& v) {
            x = v.x; y = v.y; z = 0; w = 0;
            return *this;
        }

        // ===== �����Ӽ� =====
        Vector4<T> operator+(const Vector4<T>& v) const {
            return Vector4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
        }

        Vector4<T>& operator+=(const Vector4<T>& v) {
            x += v.x; y += v.y; z += v.z; w += v.w;
            return *this;
        }

        Vector4<T> operator-(const Vector4<T>& v) const {
            return Vector4<T>(x - v.x, y - v.y, z - v.z, w - v.w);
        }

        Vector4<T>& operator-=(const Vector4<T>& v) {
            x -= v.x; y -= v.y; z -= v.z; w -= v.w;
            return *this;
        }

        // ===== ȡ���� =====
        Vector4<T> operator-() const {
            return Vector4<T>(-x, -y, -z, -w);
        }

        // ===== �����˳� =====
        Vector4<T> operator*(T s) const {
            return Vector4<T>(x * s, y * s, z * s, w * s);
        }

        Vector4<T>& operator*=(T s) {
            x *= s; y *= s; z *= s; w *= s;
            return *this;
        }

        Vector4<T> operator/(T s) const {
            assert(s != 0);
            T inv = static_cast<T>(1) / s;
            return Vector4<T>(x * inv, y * inv, z * inv, w * inv);
        }

        Vector4<T>& operator/=(T s) {
            assert(s != 0);
            T inv = static_cast<T>(1) / s;
            x *= inv; y *= inv; z *= inv; w *= inv;
            return *this;
        }

        // ===== �Ƚ� =====
        bool operator==(const Vector4<T>& v) const {
            return x == v.x && y == v.y && z == v.z && w == v.w;
        }

        bool operator!=(const Vector4<T>& v) const {
            return !(*this == v);
        }

        // ===== ��ӡ =====
        void print() const {
            std::cout << "Vector4(" << x << ", " << y << ", " << z << ", " << w << ")" << std::endl;
        }

    public:
        T x, y, z, w;
    };

    using vec2f = Vector2<float>;
    using vec2i = Vector2<int>;
    using vec3f = Vector3<float>;
    using vec3i = Vector3<int>;
    using vec4f = Vector4<float>;
    using vec4i = Vector4<int>;
}