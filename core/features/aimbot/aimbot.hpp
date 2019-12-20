#pragma once
#include "../../../dependencies/common_includes.hpp"

class c_aimbot {
private:
	int find_target(c_usercmd* user_cmd) noexcept;
	int get_nearest_bone(player_t* entity, c_usercmd* user_cmd) noexcept;
	void weapon_settings(weapon_t* weapon) noexcept;
	bool aim_auto_fire;
	int aim_mode;
	float aim_smooth;
	float aim_fov;
	float rcs_x;
	float rcs_y;
	int hitbox_id;
	vec3_t angle;
	int kill_delay;
public:
	void run(c_usercmd*) noexcept;
	void event_player_death(i_game_event* event) noexcept;
	void auto_pistol(c_usercmd* user_cmd);
	void rcs_standalone(c_usercmd* user_cmd) noexcept;
};

extern c_aimbot aimbot;