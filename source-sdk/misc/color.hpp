#pragma once

struct color {
	color() = default;
	color(int r, int g, int b, int a = 255) : r(r), g(g), b(b), a(a) { }

	static color black(int a = 255) { return { 0, 0, 0, a }; }
	static color white(int a = 255) { return { 255, 255, 255, a }; }
	static color red(int a = 255) { return { 255, 0, 0, a }; }
	static color green(int a = 255) { return { 0, 255, 0, a }; }
	static color blue(int a = 255) { return { 0, 0, 255, a }; }

	int r, g, b, a;
};