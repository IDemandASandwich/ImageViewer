#pragma once

class H_edge; //forward declaration

class Vertex {
public:
	double x, y, z;
	H_edge* edge;

	Vertex() : x(0), y(0), z(0), edge(nullptr) {};
	void set(QVector3D v, H_edge* e) {
		x = v.x();
		y = v.y();
		z = v.z();
		edge = e;
	};
	bool operator==(const Vertex& other) const {
		return (x == other.x && y == other.y && z == other.z);
	}
};

class Face {
public:
	H_edge* edge;

	Face() : edge(nullptr) {};
};

class H_edge {
public:
	Vertex* vert_origin;
	Face* face;
	H_edge* edge_prev, * edge_next;
	H_edge* pair;

	H_edge() : vert_origin(nullptr), face(nullptr), edge_prev(nullptr), edge_next(nullptr), pair(nullptr) {};
	void set(Vertex* vo, Face* f, H_edge* ep, H_edge* en, H_edge* p) {
		vert_origin = vo;
		face = f;
		edge_prev = ep;
		edge_next = en;
		pair = p;
	}
};