#include "renderer.hpp"

#include "../utilities.hpp"
#include "../console/console.hpp"
#include "../../interfaces/interfaces.hpp"
#include "../../math/math.hpp"
#include "../../../dependencies/interfaces/i_surface.hpp"
#include "../../../source-sdk/misc/color.hpp"
#include "../../../source-sdk/math/vector2d.hpp"
#include "../../../source-sdk/math/vector3d.hpp"


void render::initialize() {
	render::fonts::main = interfaces::surface->font_create();
	render::fonts::pixel = interfaces::surface->font_create();
	render::fonts::pixel_shadow = interfaces::surface->font_create();

	interfaces::surface->set_font_glyph(render::fonts::main, "Tahoma", 12, 400, 0, 0, font_flags::fontflag_dropshadow | font_flags::fontflag_antialias);
	interfaces::surface->set_font_glyph(render::fonts::pixel, "Smallest Pixel-7", 11, 400, 0, 0, font_flags::fontflag_none);
	interfaces::surface->set_font_glyph(render::fonts::pixel_shadow, "Smallest Pixel-7", 11, 400, 0, 0, font_flags::fontflag_dropshadow);

	console::log("[setup] render initialized!\n");
}

void render::draw_line(int x1, int y1, int x2, int y2, const color& c) {
	interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_line(x1, y1, x2, y2);
}

void render::draw_text_wchar(int x, int y, unsigned long font, const wchar_t* string, bool text_centered, const color& c) {
	int width, height;
	interfaces::surface->get_text_size(font, string, width, height);
	interfaces::surface->set_text_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_text_font(font);
	if (text_centered)
		interfaces::surface->draw_text_pos(x - (width / 2), y);
	else
		interfaces::surface->draw_text_pos(x, y);
	interfaces::surface->draw_render_text(string, wcslen(string));
}

void render::draw_text_string(int x, int y, unsigned long font, std::string string, bool text_centered, const color& c) {
	const auto converted_text = std::wstring(string.begin(), string.end());

	int width, height;
	interfaces::surface->get_text_size(font, converted_text.c_str(), width, height);

	interfaces::surface->set_text_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_text_font(font);
	interfaces::surface->draw_text_pos(text_centered ? x - (width / 2) : x, y);

	interfaces::surface->draw_render_text(converted_text.c_str(), wcslen(converted_text.c_str()));
}

void render::draw_rect(int x, int y, int w, int h, const color& c) {
	interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_outlined_rect(x, y, w, h);
}

void render::draw_filled_rect(int x, int y, int w, int h, const color& c) {
	interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_filled_rectangle(x, y, w, h);
}

void render::draw_outline(int x, int y, int w, int h, const color& c) {
	interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_outlined_rect(x, y, w, h);
}

void render::draw_textured_polygon(int n, vertex_t* vertice, const color& c) {
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	unsigned int texture_id{};
	if (!texture_id) {
		texture_id = interfaces::surface->create_new_texture_id(true);
		interfaces::surface->set_texture_rgba(texture_id, buf, 1, 1);
	}
	interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
	interfaces::surface->set_texture(texture_id);
	interfaces::surface->draw_polygon(n, vertice);
}

void render::draw_circle(int x, int y, int r, int s, const color& c) {
	const float step = static_cast<float>(m_pi) * 2.f / s;

	for (float a = 0.f; a < static_cast<float>(m_pi) * 2.f; a += step) {
		const float x1 = r * std::cos(a) + x;
		const float y1 = r * std::sin(a) + y;
		const float x2 = r * std::cos(a + step) + x;
		const float y2 = r * std::sin(a + step) + y;

		interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
		interfaces::surface->draw_line(x1, y1, x2, y2);
	}
}

void render::draw_circle_3d(int x, int y, int z, int r, int s, color col, bool rainbow) {
	float step = static_cast<float>(m_pi) * 2.f / s;

	for (float a = 0; a < (static_cast<float>(m_pi) * 2.f); a += step) {
		int deg = static_cast<int>(a * (180.f / static_cast<float>(m_pi)));
		vec3_t v_start{ r * cosf(a) + x, r * sinf(a) + y, z };
		vec3_t v_end{ r * cosf(a + step) + x, r * sinf(a + step) + y, z };
		vec2_t w2s_start, w2s_end;

		if (math::world_to_screen(v_start, w2s_start) && math::world_to_screen(v_end, w2s_end)) {
			if (rainbow) {
				color drawing_color = utilities::color_from_hsv(deg, 1.f, 1.f);
				interfaces::surface->set_drawing_color(drawing_color.r, drawing_color.g, drawing_color.b, col.a);
			}
			else
				interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a);

			interfaces::surface->draw_line(w2s_start.x, w2s_start.y, w2s_end.x, w2s_end.y);
		}		
	}
}

void render::draw_xhair(int x, int y, bool outline, const color& c) {
	if (outline) {
		interfaces::surface->set_drawing_color(0, 0, 0, c.a);
		interfaces::surface->draw_filled_rectangle(x - 3, y - 1, 7, 3);
		interfaces::surface->draw_filled_rectangle(x - 1, y - 3, 3, 7);
	}

	interfaces::surface->set_drawing_color(c.r, c.g, c.b, c.a);
	interfaces::surface->draw_filled_rectangle(x - 2, y, 5, 1);
	interfaces::surface->draw_filled_rectangle(x, y - 2, 1, 5);
}

color gradient(color Input, color Input2, float fraction) {
	return color(
		((Input.r - Input2.r) * fraction) + Input2.r,
		((Input.g - Input2.g) * fraction) + Input2.g,
		((Input.b - Input2.b) * fraction) + Input2.b,
		((Input.a - Input2.a) * fraction) + Input2.a);
}

void render::draw_gradient(const color& c1, const color& c2, int x, int y, int w, int h, bool horizontal) {
	if (horizontal) {
		for (int i = 0; i < w; ++i) {
			color draw_color = gradient(c1, c2, static_cast<float>(i / w));
			interfaces::surface->set_drawing_color(draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			interfaces::surface->draw_filled_rectangle(x + i, y, 1, h);
		}
	}
	else {
		for (int i = 0; i < h; ++i) {
			color draw_color = gradient(c1, c2, static_cast<float>(i / h));
			interfaces::surface->set_drawing_color(draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			interfaces::surface->draw_filled_rectangle(x, y + i, w, 1);
		}
	}
}

vec2_t render::get_text_size(unsigned long font, std::string text) {
	std::wstring a(text.begin(), text.end());
	const wchar_t* wstr = a.c_str();
	static int w, h;

	interfaces::surface->get_text_size(font, wstr, w, h);
	return { static_cast<float>(w), static_cast<float>(h) };
}