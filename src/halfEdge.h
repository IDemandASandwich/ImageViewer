#pragma once

#include <QVector3D>

class H_edge; //forward declaration

class Vertex {
public:
	double x, y, z;
	H_edge* edge;

	Vertex() : x(0), y(0), z(0), edge(nullptr) {};
	Vertex(QVector3D v, H_edge* e = nullptr) : x(v.x()), y(v.y()), z(v.z()), edge(e) {};
	void set(QVector3D v, H_edge* e) {
		x = v.x();
		y = v.y();
		z = v.z();
		edge = e;
	};
	bool operator==(const Vertex& other) const {
		return (x == other.x && y == other.y && z == other.z);
	}
	friend QDebug operator<<(QDebug debug, const Vertex& v) {
		debug << v.x << v.y << v.z;
		return debug;
	}
};

class Face {
public:
	H_edge* edge;

	Face() : edge(nullptr) {};
	Face(H_edge* e) : edge(e) {};
};

class H_edge {
public:
	Vertex* vert_origin;
	Face* face;
	H_edge* edge_prev, * edge_next;
	H_edge* pair;

	H_edge() : vert_origin(nullptr), face(nullptr), edge_prev(nullptr), edge_next(nullptr), pair(nullptr) {};
	H_edge(Vertex* vo, Face* f, H_edge* ep = nullptr, H_edge* en = nullptr, H_edge* p = nullptr) :vert_origin(vo), face(f), edge_prev(ep), edge_next(en), pair(p) {};
	void set(Vertex* vo, Face* f, H_edge* ep = nullptr, H_edge* en = nullptr, H_edge* p = nullptr) {
		vert_origin = vo;
		face = f;
		edge_prev = ep;
		edge_next = en;
		pair = p;
	}
};