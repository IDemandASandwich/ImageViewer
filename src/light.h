#pragma once

class light {
public:
	unsigned char r, g, b;
	double coeffR, coeffG, coeffB;
	int x, y, z;

	light() :r(0), g(0), b(0), coeffR(0), coeffG(0), coeffB(0), x(0), y(0), z(0) {};
	light(unsigned char r, unsigned char g, unsigned char b, int x, int y, int z)
		:r(r), g(g), b(b), x(x), y(y), z(z), coeffR(0), coeffG(0), coeffB(0) {};
	light(unsigned char r, unsigned char g, unsigned char b, double cr, double cg, double cb)
		:r(r), g(g), b(b), coeffR(cr), coeffG(cg), coeffB(cb), x(0), y(0), z(0) {};
	light(double cr, double cg, double cb)
		:coeffR(cr), coeffG(cg), coeffB(cb), r(0), g(0), b(0), x(0), y(0), z(0) {};
};

class lighting {
public:
	light source, ambient, diffusion, reflection;
	lighting(light s, light a, light d, light r) :source(s), ambient(a), diffusion(d), reflection(r) {};
};