#pragma once

#include <filesystem>

class c_config final {
public:
	void run(const char*) noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;
	void reset() noexcept;

	constexpr auto& get_configs() noexcept {
		return configs;
	}

	struct items_db {
		struct ragebot_db {
			//aimbot groupbox
			bool activate{ false };
			bool automaticfire{ false };

			bool hitbox_head{ false };
			bool hitbox_neck{ false };
			bool hitbox_chest{ false };
			bool hitbox_stomach{ false };
			bool hitbox_arms{ false };
			bool hitbox_legs{ false };
			bool hitbox_feet{ false };

			bool avoid_limbs_moving{ false };
			bool avoid_head_jumping{ false };

			bool multipoint_head{ false };
			bool multipoint_body{ false };
			float multipoint_head_scale{ 50 };
			float multipoint_body_scale{ 50 };

			bool hitchance_enable{ false };
			int hitchance_amount{ 50 };
			int minimum_damage{ 25 };

			//other groupbox
			bool force_bodyaim{ false };
			bool backtrack{ false };
			int backtrack_type{ 0 };
			bool bodyaim_if_lethal{ false };
			bool bodyaim_if_inair{ false };
			bool bodyaim_if_missedshots{ false };
			int bodyaim_missedshots{ 0 };

			bool antiaim_correction{ false };

			bool automatic_scope{ false };
			bool automatic_stop{ false };
			int automatic_stop_type{ 0 };

			//fake-lag groupbox
			bool fakelag{ false };
			bool fakelag_standing{ false };
			bool fakelag_moving{ false };
			bool fakelag_inair{ false };
			bool fakelag_shooting{ false };
			int fakelag_type{ 0 };
			int fakelag_ticks{ 0 };
			bool fakeduck{ false };
		} ragebot;

		struct legitbot_db {
			bool aim_enabled{ false };
			int activation_type{ 0 };
			int aim_key{ 0 };
			bool scope_aim{ false };
			bool smoke_check{ false };
			bool aim_silent{ false };
			bool aim_distance_based_fov{ false };
			float aim_fov_pistol{ 0.0f };
			float rcs_x_pistol{ 0.0f };
			float rcs_y_pistol{ 0.0f };
			float rcs_x_rifle{ 0.0f };
			float rcs_y_rifle{ 0.0f };
			float rcs_x_sniper{ 0.0f };
			float rcs_y_sniper{ 0.0f };
			float rcs_x_heavy{ 0.0f };
			float rcs_y_heavy{ 0.0f };
			float rcs_x_smg{ 0.0f };
			float rcs_y_smg{ 0.0f };
			float rcs_standalone_x{ 0.0f };
			float rcs_standalone_y{ 0.0f };
			float aim_smooth_pistol{ 5.f };
			float aim_fov_rifle{ 0.0f };
			float aim_smooth_rifle{ 5.f };
			float aim_fov_sniper{ 0.0f };
			float aim_smooth_sniper{ 5.f };
			float aim_fov_heavy{ 0.0f };
			float aim_smooth_heavy{ 5.f };
			float aim_fov_smg{ 0.0f };
			float aim_smooth_smg{ 5.f };
			int aim_bone_smg{ 0 };
			bool aim_auto_fire_smg{ false };
			int aim_mode_smg{ 0 };
			int aim_bone_sniper{ 0 };
			bool aim_auto_fire_sniper{ false };
			int aim_mode_sniper{ 0 };
			int aim_bone_pistol{ 0 };
			bool aim_auto_fire_pistol{ false };
			int aim_mode_pistol{ 0 };
			int aim_bone_rifle{ 0 };
			bool aim_auto_fire_rifle{ false };
			int aim_mode_rifle{ 0 };
			int aim_bone_heavy{ 0 };
			bool aim_auto_fire_heavy{ false };
			int aim_mode_heavy{ 0 };
			int aim_weapon{ 0 };
			bool aim_team_check{ false };
			int aimbot_delay_after_kill{ 0 };
			bool aimbot_auto_pistol{ false };
			bool jump_check{ false };
			bool jump_check_client{ false };
			bool jump_check_target{ false };
			bool backtrack{ false };
			bool aim_at_backtrack{ false };
			float backtrack_ms{ 0.0f };
		} legitbot;

		struct visuals_db {
			bool active{ false };
			int activation_type{ 0 };
			int activation_key{ 0 };
			bool team_check{ false };
			bool visible_only_check{ false };

			bool player_name{ false };
			bool player_name_isfake{ false };
			bool player_dormant{ false };
			bool player_ammo{ false };
			bool player_flags{ false };
			bool player_flags_money{ false };
			bool player_flags_armor{ false };
			bool player_flags_c4{ false };
			bool player_flags_flashed{ false };
			bool player_flags_scoped{ false };
			bool player_flags_defuser{ false };
			bool player_flags_reloading{ false };
			bool player_flags_callout{ false };
			int player_box{ 0 };
			bool player_health{ false };
			bool player_weapon{ false };
			bool player_flash_bar{ false };
			bool player_armor{ false };
			bool sound_footstep{ false };
			bool skeleton{ false };
			bool backtrack_skeleton{ false };
			bool preview_visuals{ false };

			bool force_crosshair{ false };
			bool crosshair_color{ false };
			bool crosshair_outline_color{ false };
			float clr_crosshair[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_crosshair_outline[4]{ 1.0f,1.0f,1.0f,1.0f };
			bool draw_fov_circle{ false };
			int viewmodel_field_of_view{ 0 };
			int field_of_view{ 0 };
			bool nightmode{ false };
			int nightmode_brightness{ 100 };

			bool glow{ false };
			bool glow_enemy{ false };
			bool glow_team{ false };
			bool glow_planted{ false };
			bool glow_weapons{ false };
			float clr_glow[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_glow_team[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_glow_planted[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_glow_dropped[4]{ 1.0f,1.0f,1.0f,1.0f };

			bool chams{ false };
			int vis_chams_type{ 0 };
			bool vis_chams_vis{ false };
			bool vis_chams_invis{ false };
			bool vis_chams_smoke_check{ false };
			bool vis_chams_vis_teammate{ false };
			bool vis_chams_invis_teammate{ false };
			float clr_chams_vis[4]{ 0.0f,0.58f,0.78f,0.5f };
			float clr_chams_invis[4]{ 0.0f,1.0f,0.39f,0.5f };
			float clr_chams_vis_teammate[4]{ 1.0f,0.0f,1.0f,0.5f };
			float clr_chams_invis_teammate[4]{ 0.68f,0.0f,1.0f,0.5f };

			bool chams_sleeve{ false };
			float clr_chams_sleeve[4]{ 115.f / 255.f, 15.f / 255.f, 185.0f / 255.f,0.5f };
			bool chams_weapon{ false };
			float clr_chams_weapon[4]{ 115.f / 255.f, 15.f / 255.f, 185.0f / 255.f,0.5f };
			bool chams_arms{ false };
			float clr_chams_arms[4]{ 115.f / 255.f, 15.f / 255.f, 185.0f / 255.f,0.5f };

			bool backtrack_visualize{ false };

			bool remove_smoke{ false };
			float clr_sky[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_box[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_name[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_weapon[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_flash_bar[4]{ 1.0f,1.0f,1.0f,1.0f };
			float clr_footstep[4]{ 1.0f,1.0f,1.0f,1.0f };

			bool reduce_flash{ false };
			bool remove_sleeves{ false };
			bool remove_hands{ false };
			bool remove_scope{ false };
			bool bomb_planted{ false };
			bool dropped_weapons{ false };
			bool danger_zone_dropped{ false };
			bool entity_esp{ false };
			bool projectiles{ false };
		} visuals;

		bool anti_screenshot{ false };
		bool viewmodel_offset{ false };
		int viewmodel_x{ 4 };
		int viewmodel_y{ 5 };
		int viewmodel_z{ -4 };
		int knife_wear{ 0 };
		float flash_alpha{ 255.f };
		bool spectators_list{ false };
		bool watermark{ true };
		bool misc_enabled{ false };
		bool misc_save_config{ false };
		bool misc_load_config{ false };
		bool bunny_hop{ false };
		bool bunny_hop_humanize{ false };
		bool bunny_hop_minimum{ false };
		bool bunny_hop_maximum{ false };
		int bunny_hop_minimum_value{ 0 };
		int bunny_hop_maximum_value{ 0 };
		int bunny_hop_hitchance{ 0 };
		bool auto_strafe{ false };
		bool clan_tag{ false };
		bool radar{ false };
		bool hitmarker{ false };
		bool logs_player_hurt{ false };
		bool logs_player_bought{ false };
		bool logs_config_system{ true };
		bool skinchanger_enable{ false };
		int hitmarker_sound{ 1 };
		int knife_model{ 2 };
		int knife_skin{ 0 };
		int config_selection{ 0 };
		int keybinds_selection{ 0 };
		bool edge_jump{ false };
		bool edge_jump_duck_in_air{ false };
		int edge_jump_key{ 0 };
		int changer_skin{ 0 };
		// knife
		int paint_kit_vector_index_knife{ 0 };
		int paint_kit_index_knife{ 0 };
		// pistol
		int paint_kit_vector_index_usp{ 0 };
		int paint_kit_index_usp{ 0 };
		int paint_kit_vector_index_p2000{ 0 };
		int paint_kit_index_p2000{ 0 };
		int paint_kit_vector_index_glock{ 0 };
		int paint_kit_index_glock{ 0 };
		int paint_kit_vector_index_p250{ 0 };
		int paint_kit_index_p250{ 0 };
		int paint_kit_vector_index_fiveseven{ 0 };
		int paint_kit_index_fiveseven{ 0 };
		int paint_kit_vector_index_tec{ 0 };
		int paint_kit_index_tec{ 0 };
		int paint_kit_vector_index_cz{ 0 };
		int paint_kit_index_cz{ 0 };
		int paint_kit_vector_index_duals{ 0 };
		int paint_kit_index_duals{ 0 };
		int paint_kit_vector_index_deagle{ 0 };
		int paint_kit_index_deagle{ 0 };
		int paint_kit_vector_index_revolver{ 0 };
		int paint_kit_index_revolver{ 0 };
		// rifle
		int paint_kit_vector_index_famas{ 0 };
		int paint_kit_index_famas{ 0 };
		int paint_kit_vector_index_galil{ 0 };
		int paint_kit_index_galil{ 0 };
		int paint_kit_vector_index_m4a4{ 0 };
		int paint_kit_index_m4a4{ 0 };
		int paint_kit_vector_index_m4a1{ 0 };
		int paint_kit_index_m4a1{ 0 };
		int paint_kit_vector_index_ak47{ 0 };
		int paint_kit_index_ak47{ 0 };
		int paint_kit_vector_index_sg553{ 0 };
		int paint_kit_index_sg553{ 0 };
		int paint_kit_vector_index_aug{ 0 };
		int paint_kit_index_aug{ 0 };
		// sniper
		int paint_kit_vector_index_ssg08{ 0 };
		int paint_kit_index_ssg08{ 0 };
		int paint_kit_vector_index_awp{ 0 };
		int paint_kit_index_awp{ 0 };
		int paint_kit_vector_index_scar{ 0 };
		int paint_kit_index_scar{ 0 };
		int paint_kit_vector_index_g3sg1{ 0 };
		int paint_kit_index_g3sg1{ 0 };
		// smg
		int paint_kit_vector_index_mac10{ 0 };
		int paint_kit_index_mac10{ 0 };
		int paint_kit_vector_index_mp5sd{ 0 };
		int paint_kit_index_mp5sd{ 0 };
		int paint_kit_vector_index_mp7{ 0 };
		int paint_kit_index_mp7{ 0 };
		int paint_kit_vector_index_mp9{ 0 };
		int paint_kit_index_mp9{ 0 };
		int paint_kit_vector_index_ppbizon{ 0 };
		int paint_kit_index_ppbizon{ 0 };
		int paint_kit_vector_index_p90{ 0 };
		int paint_kit_index_p90{ 0 };
		int paint_kit_vector_index_ump45{ 0 };
		int paint_kit_index_ump45{ 0 };
		// heavy
		int paint_kit_vector_index_mag7{ 0 };
		int paint_kit_index_mag7{ 0 };
		int paint_kit_vector_index_nova{ 0 };
		int paint_kit_index_nova{ 0 };
		int paint_kit_vector_index_sawedoff{ 0 };
		int paint_kit_index_sawedoff{ 0 };
		int paint_kit_vector_index_xm1014{ 0 };
		int paint_kit_index_xm1014{ 0 };
		int paint_kit_vector_index_m249{ 0 };
		int paint_kit_index_m249{ 0 };
		int paint_kit_vector_index_negev{ 0 };
		int paint_kit_index_negev{ 0 };
		// gloves
		bool glovechanger_enable{ false };
		int glove_model{ 0 };
		int glove_skin{ 0 };
		int paint_kit_vector_index_glove{ 0 };
		int paint_kit_index_glove{ 0 };
		int glove_wear{ 0 };
		//
		bool disable_post_processing{ false };
		bool recoil_crosshair{ false };
		bool rank_reveal{ false };
	} item;

private:
	std::filesystem::path path;
	std::vector<std::string> configs;
};

extern c_config config_system;