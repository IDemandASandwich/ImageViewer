#pragma once

#include <qvector3d.h>

class W_edge; //Forward declaration
class WVertex {
public:
	double x, y, z;
	W_edge* edge;

	WVertex() : x(0), y(0), z(0), edge(nullptr) {};
	WVertex(QVector3D v, W_edge* e) : x(v.x()), y(v.y()), z(v.z()), edge(e) {};
};
class WFace {
public:
	W_edge* edge;
	bool right_oriented;

	WFace() : edge(nullptr), right_oriented(false) {};
	WFace(W_edge* e, bool r) : edge(e), right_oriented(r) {};
};
class W_edge {
public:
	WVertex* vert_origin, * vert_destination;
	WFace* face_left, * face_right;
	W_edge* edge_left_prev, * edge_left_next;
	W_edge* edge_right_prev, * edge_right_next;

	W_edge() : vert_origin(nullptr), vert_destination(nullptr), face_left(nullptr), face_right(nullptr), edge_left_prev(nullptr), edge_left_next(nullptr), edge_right_prev(nullptr), edge_right_next(nullptr) {};
	W_edge(WVertex* vo, WVertex* vd, WFace* fl, WFace* fr, W_edge* elp, W_edge* eln, W_edge* erp, W_edge* ern) :
		vert_origin(vo), vert_destination(vd), face_left(fl), face_right(fr), edge_left_prev(elp), edge_left_next(eln), edge_right_prev(erp), edge_right_next(ern) {};
};