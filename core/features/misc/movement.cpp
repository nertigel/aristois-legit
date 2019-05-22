#include "../../../dependencies/common_includes.hpp"
#include "movement.hpp"

c_movement movement;

auto flags_backup = 0;

void c_movement::bunnyhop(c_usercmd* user_cmd) noexcept {
	if (!config_system.bunny_hop || !config_system.misc_enabled)
		return;

	float hitchance = c_config::get().bunny_hop_hitchance;
		float restrictlimit = 12;
		float hoplimit = c_config::get().bunny_hop_maximum_value;
		float minhop = c_config::get().bunny_hop_minimum_value;


		static int hops_restricted = 0;
		static int hops_hit = 0;
		if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
			return;

		if (user_cmd->buttons & in_jump && !(local_player->flags() & fl_onground))
		{
			user_cmd->buttons &= ~in_jump;
			hops_restricted = 0;
		}

		else if ((rand() % 100 > hitchance && hops_restricted < restrictlimit) || (hoplimit > 0 && hops_hit > hoplimit && minhop > 0 && hops_hit > minhop))
		{
			user_cmd->buttons &= ~in_jump;
			hops_restricted++;
			hops_hit = 0;
		}
		else
		{
			hops_hit++;
		}
}

void c_movement::edge_jump_pre_prediction(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.edge_jump)
		return;

	if (!GetAsyncKeyState(config_system.edge_jump_key))
		return;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	flags_backup = local_player->flags();
}

void c_movement::edge_jump_post_prediction(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.edge_jump)
		return;

	if (!GetAsyncKeyState(config_system.edge_jump_key))
		return;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	if (flags_backup & fl_onground && !(local_player->flags() & fl_onground))
		user_cmd->buttons |= in_jump;

	if (!(local_player->flags() & fl_onground) && config_system.edge_jump_duck_in_air)
		user_cmd->buttons |= in_duck;
}
