#include "desync.hpp"
#include "../aimbot/aimbot.hpp"

c_desync desync;

float yaw_offset;
bool flip_yaw = false;
bool flip_packet = false;

ccs_player_anim_state* anim_state;

bool c_desync::is_enabled(c_usercmd* user_cmd) {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player || !local_player->is_alive())
		return false;

	if (!config_system.desync_enable || (user_cmd->buttons & in_use))
		return false;

	auto * net_channel = interfaces::engine->get_net_channel_info();
	if (net_channel && (net_channel->get_average_loss(1) > 0.f || net_channel->get_average_loss(0) > 0.f))
		return false;

	if (local_player->has_gun_game_immunity() || local_player->flags() & fl_frozen)
		return false;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return false;

	return true;
}

bool c_desync::is_firing(c_usercmd* user_cmd) {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	auto weapon = local_player->active_weapon();
	if (!weapon)
		return false;

	if (aimbot.is_grenade(weapon)) {
		return true;
	}

	else if (aimbot.is_knife(weapon)) {
		if (user_cmd->buttons & in_attack || user_cmd->buttons & in_attack2)
			return true;
	}

	else if (weapon->item_definition_index() == WEAPON_REVOLVER) {
		if (user_cmd->buttons & in_attack && can_fire())
			return true;

		if (user_cmd->buttons & in_attack2)
			return true;
	}

	else if (user_cmd->buttons & in_attack && can_fire() && !aimbot.is_bomb(weapon))
		return true;

	return false;
}

bool c_desync::can_fire() {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	auto weapon = local_player->active_weapon();
	if (!weapon)
		return false;

	if (!local_player)
		return false;

	static auto stored_tick = 0;
	static decltype(this) stored_weapon = nullptr;
	if (stored_weapon != this || stored_tick >= local_player->get_tick_base()) {
		stored_weapon = this;
		stored_tick = local_player->get_tick_base();

		return false;
	}

	if (weapon->is_reloading() || weapon->clip1_count() <= 0 || !local_player)
		return false;

	auto server_time = local_player->get_tick_base() * interfaces::globals->interval_per_tick;

	return weapon->next_primary_attack() <= server_time;
}

float c_desync::get_max_desync_delta() {
	const auto speed_factor = std::max<float>(0.f, std::min<float>(1, anim_state->m_flSpeedFactor));
	const auto speed_fraction = std::max<float>(0.f, std::min<float>(anim_state->m_flSpeedFraction, 1.f));

	const auto unk1 = (anim_state->m_flLandingRatio * -0.30000001 - 0.19999999) * speed_fraction;
	float unk2 = unk1 + 1.f;

	if (anim_state->m_flDuckAmount > 0)
		unk2 += anim_state->m_flDuckAmount * speed_factor * (0.5f - unk2);

	return anim_state->m_flMaxBodyYawDegrees * unk2;
}

void c_desync::run(c_usercmd* user_cmd, bool& send_packet) {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!send_packet)
		return;

	if (!is_enabled(user_cmd)) {
		yaw_offset = 0.f;
		return;
	}

	anim_state = local_player->get_player_anim_state();

	yaw_offset = get_max_desync_delta();

	const auto old_angles = user_cmd->viewangles;

	flip_packet = !flip_packet;
	send_packet = flip_packet;
	if (!flip_packet) {
		if (config_system.desync_flip)
			flip_yaw = !flip_yaw;
	}

	static float last_lby = 0.f;
	static float last_update = 0.f;

	const auto current_lby = local_player->lower_body_yaw();
	const float current_time = local_player->get_tick_base() * interfaces::globals->interval_per_tick;

	const float delta = ceilf((current_time - last_update) * 100) / 100;
	const auto next_delta = ceilf((delta + interfaces::globals->interval_per_tick) * 100) / 100;

	if (local_player->velocity().Length2D() <= 0.f) {
		if (current_lby != 180.f && last_lby != current_lby) {
			last_lby = current_lby;
			last_update = current_time - interfaces::globals->interval_per_tick;
		}
		else if (next_delta >= 1.1f) {
			send_packet = flip_packet = true;
			last_update = current_time;
		}
	}
	else {
		last_lby = current_lby;
		last_update = current_time;
	}

	const auto low_fps = interfaces::globals->interval_per_tick * 0.9f < interfaces::globals->absolute_frametime;
	if (low_fps || is_firing(user_cmd))
		send_packet = flip_packet = true;

	if (send_packet)
		anim_state->m_flGoalFeetYaw += flip_yaw ? yaw_offset : -yaw_offset;
	else {
		user_cmd->buttons &= ~(in_forward | in_back | in_moveright | in_moveleft);

		user_cmd->viewangles.y += 180.f;
		user_cmd->viewangles.normalize_aimbot();
		user_cmd->viewangles.clamp();
		c_math::get().correct_movement(user_cmd, old_angles);
	}
}