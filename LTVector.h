#pragma once

float epsilon = 0.0001f;
template<typename T> struct LTVector {
	T x, y, z;
	D3DXVECTOR3 ToDX() {
		return D3DXVECTOR3{ x, y, z };
	}
	LTVector() : x(0), y(0), z(0) {

	}
	LTVector(T a, T b, T c) : x(a), y(b), z(c) {

	}
	void operator=(D3DXVECTOR3 dxvec) {
		x = dxvec.x;
		y = dxvec.y;
		z = dxvec.z;
	}
	bool operator==(LTVector<T> r) {
		return abs(x - r.x) < epsilon && abs(y - r.y) < epsilon && abs(z - r.z) < epsilon;
	}
	LTVector<T> operator*(T mul) {
		return{ x * mul, y * mul, z * mul };
	}
	LTVector<T> operator-(LTVector<T> r) {
		return{ abs(x - r.x), abs(y - r.y), abs(z - r.z) };
	}
};