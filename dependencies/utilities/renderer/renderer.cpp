#include "renderer.hpp"

unsigned long render::fonts::main;
unsigned long render::fonts::pixel;
unsigned long render::fonts::pixel_shadow;

void render::initialize() {
	render::fonts::main = interfaces::surface->font_create();
	render::fonts::pixel = interfaces::surface->font_create();
	render::fonts::pixel_shadow = interfaces::surface->font_create();

	interfaces::surface->set_font_glyph(render::fonts::main, "Tahoma", 12, 400, 0, 0, font_flags::fontflag_dropshadow | font_flags::fontflag_antialias);
	interfaces::surface->set_font_glyph(render::fonts::pixel, "Smallest Pixel-7", 11, 400, 0, 0, font_flags::fontflag_none);
	interfaces::surface->set_font_glyph(render::fonts::pixel_shadow, "Smallest Pixel-7", 11, 400, 0, 0, font_flags::fontflag_dropshadow);

	console::log("[setup] render initialized!\n");
}

void render::draw_line(std::int32_t x1, std::int32_t y1, std::int32_t x2, std::int32_t y2, color colour) {
	interfaces::surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
	interfaces::surface->draw_line(x1, y1, x2, y2);
}

void render::draw_text_wchar(std::int32_t x, std::int32_t y, unsigned long font, const wchar_t* string, bool text_centered, color colour) {
	int width, height;
	interfaces::surface->get_text_size(font, string, width, height);
	interfaces::surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
	interfaces::surface->draw_text_font(font);
	if (text_centered)
		interfaces::surface->draw_text_pos(x - (width / 2), y);
	else
		interfaces::surface->draw_text_pos(x, y);
	interfaces::surface->draw_render_text(string, wcslen(string));
}

void render::draw_text_string(std::int32_t x, std::int32_t y, unsigned long font, std::string string, bool text_centered, color colour) {
	const auto converted_text = std::wstring(string.begin(), string.end());

	int width, height;
	interfaces::surface->get_text_size(font, converted_text.c_str(), width, height);

	interfaces::surface->set_text_color(colour.r, colour.g, colour.b, colour.a);
	interfaces::surface->draw_text_font(font);
	if (text_centered)
		interfaces::surface->draw_text_pos(x - (width / 2), y);
	else
		interfaces::surface->draw_text_pos(x, y);
	interfaces::surface->draw_render_text(converted_text.c_str(), wcslen(converted_text.c_str()));
}

void render::draw_rect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, color color) {
	interfaces::surface->set_drawing_color(color.r, color.g, color.b, color.a);
	interfaces::surface->draw_outlined_rect(x, y, w, h);
}

void render::draw_filled_rect(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, color colour) {
	interfaces::surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
	interfaces::surface->draw_filled_rectangle(x, y, w, h);
}

void render::draw_outline(std::int32_t x, std::int32_t y, std::int32_t w, std::int32_t h, color colour) {
	interfaces::surface->set_drawing_color(colour.r, colour.g, colour.b, colour.a);
	interfaces::surface->draw_outlined_rect(x, y, w, h);
}

void render::draw_textured_polygon(std::int32_t n, vertex_t* vertice, color col) {
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	unsigned int texture_id{};
	if (!texture_id) {
		texture_id = interfaces::surface->create_new_texture_id(true);
		interfaces::surface->set_texture_rgba(texture_id, buf, 1, 1);
	}
	interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a);
	interfaces::surface->set_texture(texture_id);
	interfaces::surface->draw_polygon(n, vertice);
}

void render::draw_circle(std::int32_t x, std::int32_t y, std::int32_t r, std::int32_t s, color col) {
	float Step = M_PI * 2.0 / s;
	for (float a = 0; a < (M_PI * 2.0); a += Step) {
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a);
		interfaces::surface->draw_line(x1, y1, x2, y2);
	}
}

void render::draw_circle_3d(std::int32_t x, std::int32_t y, std::int32_t z, std::int32_t r, std::int32_t s, color col, bool rainbow) {
	float step = M_PI * 2.0 / s;
	for (float a = 0; a < (M_PI * 2.0); a += step) {
		int deg = a * (180 / M_PI);
		vec3_t v_start = vec3_t(r * cosf(a) + x, r * sinf(a) + y,  z);
		vec3_t v_end = vec3_t(r * cosf(a + step) + x, r * sinf(a + step) + y, z);
		vec3_t w2s_start, w2s_end;
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

void render::draw_xhair(int x, int y, bool outline, color col) {
	if (outline) {
		interfaces::surface->set_drawing_color(0, 0, 0, col.a);
		interfaces::surface->draw_filled_rectangle(x - 3, y - 1, 7, 3);
		interfaces::surface->draw_filled_rectangle(x - 1, y - 3, 3, 7);
	}
	interfaces::surface->set_drawing_color(col.r, col.g, col.b, col.a);
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

void render::draw_gradient(color col, color col2, int x, int y, int w, int h, bool orientation = true) {
	if (orientation) {
		for (int i = 0; i < w; i++) {
			color draw_color = gradient(col, col2, (float)i / w);
			interfaces::surface->set_drawing_color(draw_color.r, draw_color.g, draw_color.b, draw_color.a);
			interfaces::surface->draw_filled_rectangle(x + i, y, 1, h);
		}
	}
	else {
		for (int i = 0; i < h; i++) {
			color draw_color = gradient(col, col2, (float)i / h);
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