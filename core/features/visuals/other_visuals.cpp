#include "../features.hpp"

void draw_crosshair(bool recoil, bool outline, color color) {
	int screen_size[2];
	interfaces::surface->get_screen_size(screen_size[0], screen_size[1]); //get current screen size

	auto x = screen_size[0] / 2; //divide screen size with two so our xhair is in the middle of the screen
	auto y = screen_size[1] / 2;
	if (recoil) { //should the crosshair follow the recoil?
		vec3_t punch = csgo::local_player->aim_punch_angle() / (csgo::local_player->is_scoped() ? .5f : 1.07f); //get the aim punch/recoil amount (fixed when scoped, with aug,sg)
		auto dx = screen_size[0] / 90;
		auto dy = screen_size[1] / 90;
		x -= ((screen_size[0] / 90) * (punch.y)); //subtract the punch from the x-, y-position of the xhair
		y += ((screen_size[1] / 90) * (punch.x));
	}
	render::draw_xhair(x, y, outline, color); // draw that b*tch
}

void visuals::other_visuals::draw() {
	if (csgo::local_player->is_alive()) { //only draw when alive
		if (variables::visuals::crosshair_enable)
			draw_crosshair(variables::visuals::crosshair_recoil, variables::visuals::crosshair_outline, variables::visuals::crosshair_color);
	}
}