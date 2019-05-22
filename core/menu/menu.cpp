#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../features/misc/logs.hpp"
#include "../features/skinchanger/parser.hpp"

c_menu menu;

#define UNLEN 256
IDirect3DStateBlock9 *state_block;
bool reverse = false;
int offset = 0;
bool show_popup = false;
static bool save_config = false;
static bool load_config = false;
namespace ImGui {
	static auto vector_getter = [](void* vec, int idx, const char** out_text) {
		auto& vector = *static_cast<std::vector<std::string>*>(vec);
		if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
		*out_text = vector.at(idx).c_str();
		return true;
	};

	bool combo_array(const char* label, int* current_index, std::vector<std::string>& values) {
		if (values.empty()) { return false; }
		return Combo(label, current_index, vector_getter,
			static_cast<void*>(&values), values.size());
	}

	long get_mils() {
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	void begin_popup(const char* text, int onScreenMils, bool* done) {
		if (!done)
			show_popup = true;

		ImGuiIO &io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		int width = io.DisplaySize.x;
		static long old_time = -1;
		ImGui::SetNextWindowPos(ImVec2(width - offset, 100));
		style.WindowMinSize = ImVec2(100.f, 20.f);
		ImGui::Begin("##PopUpWindow", &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 15, p.y - 13), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		long current_time_ms = get_mils();

		ImVec2 text_size = ImGui::CalcTextSize(text);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - text_size.y / 2);
		ImGui::Text(text);

		if (!reverse) {
			if (offset < ImGui::GetWindowWidth())
				offset += (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);

			if (offset >= ImGui::GetWindowWidth() && old_time == -1) {
				old_time = current_time_ms;
			}
		}

		if (current_time_ms - old_time >= onScreenMils && old_time != -1)
			reverse = true;

		if (reverse) {
			if (offset > 0)
				offset -= (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);
			if (offset <= 0) {
				offset = 0;
				reverse = false;
				*done = true;
				old_time = -1;
				show_popup = false;
			}
		}

		ImGui::End();
	}
}

void c_menu::run() {
	static int page = 0;

	if (opened) {
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(167 / 255.f, 24 / 71, 247 / 255.f, 1.f);

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("aristois", &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImColor c = ImColor(32, 114, 247);

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 30), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 3), ImColor(30, 30, 39));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));
			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7); 
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7); 
			ImGui::Text("aristois.me");
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9);
			if (ImGui::ButtonT("legit", ImVec2(40, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("visuals", ImVec2(40, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("misc", ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("skins", ImVec2(40, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("config", ImVec2(40, 30), page, 4, false, false)) page = 4; ImGui::SameLine(0, 0);
			ImGui::PopFont();

			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222); 

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

			if (config_system.visuals_preview) {
				run_visuals_preview();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			static int test = 0;
			switch (page) {
			case 0:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				//push window color for child
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				//push border color for child
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("aimbot", ImVec2(279, 268), true); {
					ImGui::Checkbox("active", &config_system.aim_enabled);
					ImGui::Combo("mode", &config_system.aim_mode, "hitbox\0nearest hitbox"); //todo add custom bone selection - designer
					ImGui::Checkbox("dynamic fov", &config_system.aim_distance_based_fov);
					ImGui::Checkbox("silent aim", &config_system.aim_silent);
					ImGui::Checkbox("scope aim", &config_system.scope_aim);
					ImGui::Checkbox("smoke aim", &config_system.smoke_check);
					ImGui::Checkbox("friendly fire", &config_system.aim_team_check);
				} ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("backtrack", ImVec2(279, 267), true); {
					ImGui::Checkbox("active", &config_system.backtrack);
					ImGui::Checkbox("aim at records", &config_system.aim_at_backtrack);
					ImGui::SliderFloat("backtrack records (ms)", &config_system.backtrack_ms, 1.0f, 200.0f, "%.2f");
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("settings", ImVec2(279, 543), true); {
					if (ImGui::ButtonT("pistol", ImVec2(50, 30), test, 0, false, ImColor(0, 0, 0))) test = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("rifle", ImVec2(50, 30), test, 1, false, false)) test = 1; ImGui::SameLine(0, 0); 
					if (ImGui::ButtonT("sniper", ImVec2(50, 30), test, 2, false, false)) test = 2; ImGui::SameLine(0, 0); 
					if (ImGui::ButtonT("smg", ImVec2(50, 30), test, 3, false, false)) test = 3; ImGui::SameLine(0, 0); 
					if (ImGui::ButtonT("heavy", ImVec2(50, 30), test, 4, false, false)) test = 4; 

					ImGui::PushFont(font_menu);

					switch (test) {
					case 0:
						if (config_system.aim_mode == 0) {
							ImGui::Combo("pistol hitbox", &config_system.aim_bone_pistol, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("pistol fov", &config_system.aim_fov_pistol, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("pistol smooth", &config_system.aim_smooth_pistol, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("pistol rcs x", &config_system.rcs_x_pistol, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("pistol rcs y", &config_system.rcs_y_pistol, 0.0f, 1.0f, "%.2f");
						break;
					case 1:
						if (config_system.aim_mode == 0) {
							ImGui::Combo("rifle hitbox", &config_system.aim_bone_rifle, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("rifle fov", &config_system.aim_fov_rifle, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("rifle smooth", &config_system.aim_smooth_rifle, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("rifle rcs x", &config_system.rcs_x_rifle, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("rifle rcs y", &config_system.rcs_y_rifle, 0.0f, 1.0f, "%.2f");
						break;
					case 2:
						if (config_system.aim_mode == 0) {
							ImGui::Combo("sniper hitbox", &config_system.aim_bone_sniper, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("sniper fov", &config_system.aim_fov_sniper, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("sniper smooth", &config_system.aim_smooth_sniper, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("sniper rcs x", &config_system.rcs_x_sniper, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("sniper rcs y", &config_system.rcs_y_sniper, 0.0f, 1.0f, "%.2f");
						break;
					case 3:
						if (config_system.aim_mode == 0) {
							ImGui::Combo("smg hitbox", &config_system.aim_bone_smg, "head\0neck\0chest\0stomach\0pelvis");
						}

						ImGui::SliderFloat("smg fov", &config_system.aim_fov_smg, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("smg smooth", &config_system.aim_smooth_smg, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("smg rcs x", &config_system.rcs_x_smg, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("smg rcs y", &config_system.rcs_y_smg, 0.0f, 1.0f, "%.2f");
						break;
					case 4:
						if (config_system.aim_mode == 0) {
							ImGui::Combo("heavy hitbox", &config_system.aim_bone_heavy, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("heavy fov", &config_system.aim_fov_heavy, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("heavy smooth", &config_system.aim_smooth_heavy, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("heavy rcs x", &config_system.rcs_x_heavy, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("heavy rcs y", &config_system.rcs_y_heavy, 0.0f, 1.0f, "%.2f");
						break;
					}

					ImGui::PopFont();
				} ImGui::EndChild(true);
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

				break;
			case 1:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("player", ImVec2(279, 268), true); {
					ImGui::Checkbox("active", &config_system.visuals_enabled);
					if (config_system.visuals_enabled) {
						ImGui::Checkbox("team", &config_system.visuals_team_check);
					}
					ImGui::Checkbox("name", &config_system.player_name);
					ImGui::ColorEdit4("name color", config_system.clr_name, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("box", &config_system.player_box);
					ImGui::ColorEdit4("box color", config_system.clr_box, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("health", &config_system.player_health);
					ImGui::Checkbox("weapon", &config_system.player_weapon);
					ImGui::ColorEdit4("weapon color", config_system.clr_weapon, ImGuiColorEditFlags_NoInputs);

					if (ImGui::BeginCombo("flags", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("armor"), &config_system.player_flags_armor, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("bomb"), &config_system.player_flags_c4, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("flashed"), &config_system.player_flags_flashed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("money"), &config_system.player_flags_money, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("scoped"), &config_system.player_flags_scoped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}
					ImGui::Checkbox("footstep", &config_system.sound_footstep);
					ImGui::ColorEdit4("footstep color", config_system.clr_footstep, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("skeleton", &config_system.skeleton);
					ImGui::Checkbox("backtrack skeleton", &config_system.backtrack_skeleton);
					ImGui::Checkbox("preview visuals", &config_system.visuals_preview);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("effects", ImVec2(279, 267), true); {
					ImGui::Checkbox("force crosshair", &config_system.force_crosshair);
					ImGui::SliderInt("viewmodel field of view", &config_system.viewmodel_fov, 0, 135);
					ImGui::SliderInt("field of view", &config_system.fov, 0, 60);
					ImGui::Checkbox("night mode", &config_system.nightmode);
					ImGui::ColorEdit4("sky color", config_system.clr_sky, ImGuiColorEditFlags_NoInputs);
					if (config_system.nightmode) {
						ImGui::SliderInt("brightness", &config_system.nightmode_brightness, 0, 100);
					}


					if (ImGui::BeginCombo("removals", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("smoke"), &config_system.remove_smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("flash"), &config_system.reduce_flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("sleeves"), &config_system.remove_sleeves, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("hands"), &config_system.remove_hands, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("scope overlay"), &config_system.remove_scope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					if (ImGui::BeginCombo("world", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("planted bomb"), &config_system.bomb_planted, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("dropped weapons"), &config_system.dropped_weapons, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("projectiles"), &config_system.projectiles, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("misc entities"), &config_system.entity_esp, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("danger zone"), &config_system.danger_zone_dropped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("glow", ImVec2(279, 268), true); {
					ImGui::Checkbox("active", &config_system.visuals_glow);
					ImGui::Checkbox("enemy", &config_system.visuals_glow_enemy);
					ImGui::ColorEdit4("glow color", config_system.clr_glow, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("teammate", &config_system.visuals_glow_team);
					ImGui::ColorEdit4("glow color team", config_system.clr_glow_team, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("planted bomb", &config_system.visuals_glow_planted);
					ImGui::ColorEdit4("glow color planted", config_system.clr_glow_planted, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("dropped weapons", &config_system.visuals_glow_weapons);
					ImGui::ColorEdit4("glow color weapons", config_system.clr_glow_dropped, ImGuiColorEditFlags_NoInputs);

				}
				ImGui::EndChild(true);

				ImGui::Spacing();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

				ImGui::BeginChild("chams", ImVec2(279, 267), true); {
					ImGui::Combo("chams type", &config_system.vis_chams_type, "textured\0flat\0metallic\0pulsating");
					ImGui::Checkbox("enemy", &config_system.vis_chams_vis);
					ImGui::ColorEdit4("enemy color", config_system.clr_chams_vis, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("enemy (behind wall)", &config_system.vis_chams_invis);
					ImGui::ColorEdit4("enemy (behind wall) color", config_system.clr_chams_invis, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("backtrack", &config_system.backtrack_visualize);
					ImGui::Checkbox("smoke check", &config_system.vis_chams_smoke_check);
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 2:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("misc", ImVec2(279, 543), true); {
					ImGui::Checkbox("active", &config_system.misc_enabled);
					ImGui::Checkbox("clantag spammer", &config_system.clan_tag);
					ImGui::Checkbox("engine radar", &config_system.radar);

					if (ImGui::BeginCombo("logs", "...", ImVec2(0, 65))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("player hurt"), &config_system.logs_player_hurt, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("player bought"), &config_system.logs_player_bought, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("config system"), &config_system.logs_config_system, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}

					ImGui::Checkbox("hitmarker", &config_system.hitmarker);
					if (config_system.hitmarker) {
						ImGui::Combo("hitmarker sound", &config_system.hitmarker_sound, "none\0one\0two\0three");
					}
					ImGui::Checkbox("anti screenshot", &config_system.anti_screenshot);
					ImGui::Checkbox("spectators", &config_system.spectators_list);
					ImGui::Checkbox("watermark", &config_system.watermark);
					ImGui::Checkbox("disable post processing", &config_system.disable_post_processing);
					ImGui::Checkbox("recoil crosshair", &config_system.recoil_crosshair);
					ImGui::Checkbox("rank reveal", &config_system.rank_reveal);

					ImGui::Checkbox("viewmodel offset", &config_system.viewmodel_offset);
					if (config_system.viewmodel_offset) {
						ImGui::SliderInt("viewmodel x", &config_system.viewmodel_x, -10, 10);
						ImGui::SliderInt("viewmodel y", &config_system.viewmodel_y, -10, 10);
						ImGui::SliderInt("viewmodel z", &config_system.viewmodel_z, -10, 10);

					}

					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("dump steam id", ImVec2(84, 18))) {
						utilities::dump_steam_id();
					}

					ImGui::SameLine();
					if (ImGui::Button("hide name", ImVec2(84, 18))) {
						utilities::change_name("\n\xAD\xAD\xAD");
					}

				}
				ImGui::EndChild(true);
				ImGui::NextColumn();

				ImGui::BeginChild("movement", ImVec2(279, 543), true); {
					ImGui::Checkbox("bunny hop", &config_system.bunny_hop);
					if (config_system.bunny_hop) {
						ImGui::SliderInt("hitchance", &config_system.bunny_hop_hitchance, 0, 100);
						ImGui::SliderInt("minimum hops", &config_system.bunny_hop_minimum_value, 0, 20);
						ImGui::SliderInt("maximum hops", &config_system.bunny_hop_maximum_value, 0, 20);
					}


					ImGui::Checkbox("edge jump", &config_system.edge_jump);
					if (config_system.edge_jump) {
						ImGui::Checkbox("duck in air", &config_system.edge_jump_duck_in_air);
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 3:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("knife", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable", &config_system.skinchanger_enable);
					ImGui::Combo("knife", &config_system.knife_model, "default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow daggers\0navaja\0stiletto\0talon\0ursus");
					ImGui::Combo("condition", &config_system.knife_wear, "factory new\0minimal wear\0field-tested\0well-worn\0battle-scarred");

					ImGui::Combo(("skin"), &config_system.paint_kit_vector_index_knife, [](void* data, int idx, const char** out_text) {
						*out_text = parser_skins[idx].name.c_str();
						return true;
					}, nullptr, parser_skins.size(), 10);
					config_system.paint_kit_index_knife = parser_skins[config_system.paint_kit_vector_index_knife].id;

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("settings", ImVec2(279, 267), true); {
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("force update", ImVec2(84, 18))) {
						utilities::force_update();
					}

				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("weapons", ImVec2(279, 543), true); {
					static int weapons_page = 0;
					if (ImGui::ButtonT("pistol", ImVec2(50, 30), weapons_page, 0, false, ImColor(0, 0, 0))) weapons_page = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("rifle", ImVec2(50, 30), weapons_page, 1, false, false)) weapons_page = 1; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("sniper", ImVec2(50, 30), weapons_page, 2, false, false)) weapons_page = 2; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("smg", ImVec2(50, 30), weapons_page, 3, false, false)) weapons_page = 3; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("heavy", ImVec2(50, 30), weapons_page, 4, false, false)) weapons_page = 4;

					switch (weapons_page) {
					case 0:
						ImGui::Combo(("p2000"), &config_system.paint_kit_vector_index_p2000, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_p2000 = parser_skins[config_system.paint_kit_vector_index_p2000].id;


						ImGui::Combo(("usp-s"), &config_system.paint_kit_vector_index_usp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_usp = parser_skins[config_system.paint_kit_vector_index_usp].id;

						ImGui::Combo(("glock"), &config_system.paint_kit_vector_index_glock, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_glock = parser_skins[config_system.paint_kit_vector_index_glock].id;

						ImGui::Combo(("p250"), &config_system.paint_kit_vector_index_p250, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_p250 = parser_skins[config_system.paint_kit_vector_index_p250].id;

						ImGui::Combo(("five-seven"), &config_system.paint_kit_vector_index_fiveseven, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_fiveseven = parser_skins[config_system.paint_kit_vector_index_fiveseven].id;

						ImGui::Combo(("tec9"), &config_system.paint_kit_vector_index_tec, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_tec = parser_skins[config_system.paint_kit_vector_index_tec].id;

						ImGui::Combo(("cz75a"), &config_system.paint_kit_vector_index_cz, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_cz = parser_skins[config_system.paint_kit_vector_index_cz].id;

						ImGui::Combo(("duals"), &config_system.paint_kit_vector_index_duals, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_duals = parser_skins[config_system.paint_kit_vector_index_duals].id;

						ImGui::Combo(("deagle"), &config_system.paint_kit_vector_index_deagle, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_deagle = parser_skins[config_system.paint_kit_vector_index_deagle].id;

						ImGui::Combo(("revolver"), &config_system.paint_kit_vector_index_revolver, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_revolver = parser_skins[config_system.paint_kit_vector_index_revolver].id;

						break;
					case 1:
						ImGui::Combo(("famas"), &config_system.paint_kit_vector_index_famas, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_famas = parser_skins[config_system.paint_kit_vector_index_famas].id;

						ImGui::Combo(("galil"), &config_system.paint_kit_vector_index_galil, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_galil = parser_skins[config_system.paint_kit_vector_index_galil].id;

						ImGui::Combo(("m4a4"), &config_system.paint_kit_vector_index_m4a4, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_m4a4 = parser_skins[config_system.paint_kit_vector_index_m4a4].id;

						ImGui::Combo(("m4a1"), &config_system.paint_kit_vector_index_m4a1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_m4a1 = parser_skins[config_system.paint_kit_vector_index_m4a1].id;

						ImGui::Combo(("ak47"), &config_system.paint_kit_vector_index_ak47, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_ak47 = parser_skins[config_system.paint_kit_vector_index_ak47].id;

						ImGui::Combo(("sg 553"), &config_system.paint_kit_vector_index_sg553, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_sg553 = parser_skins[config_system.paint_kit_vector_index_sg553].id;

						ImGui::Combo(("aug"), &config_system.paint_kit_vector_index_aug, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_aug = parser_skins[config_system.paint_kit_vector_index_aug].id;

						break;
					case 2:
						ImGui::Combo(("ssg08"), &config_system.paint_kit_vector_index_ssg08, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_ssg08 = parser_skins[config_system.paint_kit_vector_index_ssg08].id;

						ImGui::Combo(("awp"), &config_system.paint_kit_vector_index_awp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_awp = parser_skins[config_system.paint_kit_vector_index_awp].id;

						ImGui::Combo(("scar20"), &config_system.paint_kit_vector_index_scar, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_scar = parser_skins[config_system.paint_kit_vector_index_scar].id;

						ImGui::Combo(("g3sg1"), &config_system.paint_kit_vector_index_g3sg1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.paint_kit_index_g3sg1 = parser_skins[config_system.paint_kit_vector_index_g3sg1].id;

						break;
					case 3:
						break;
					case 4:
						break;

					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;

			case 4:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("config", ImVec2(279, 268), true); {
					static char entered_config_name[64] = { 0 };
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					ImGui::PushItemWidth(175);
					ImGui::InputText(("##CFG"), entered_config_name, 64);
					static int selected;
					std::string config;
					std::vector<std::string> configs = config_system.get_configs();
					if (configs.size() > 0) {
						ImGui::combo_array(("configs"), &selected, configs);
						config = configs[config_system.counts];
					}
					config_system.counts = selected;

					if (configs.size() > 0) {
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button(("load"), ImVec2(175, 20))) {
							config_system.load_config(config);
							load_config = true;

							if (config_system.logs_config_system) {
								utilities::console_warning("[config system] ");
								interfaces::console->console_printf(config.c_str());
								interfaces::console->console_printf(" loaded. \n");
								std::stringstream ss;
								ss << config.c_str() << " loaded.";
								event_logs.add(ss.str(), color(167, 255, 255, 255));
							}
						}
					}
					if (configs.size() >= 1) {
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button(("save"), ImVec2(175, 20))) {
							config_system.save_config(config);
							save_config = true;

							if (config_system.logs_config_system) {
								utilities::console_warning("[config system] ");
								interfaces::console->console_printf(config.c_str());
								interfaces::console->console_printf(" saved. \n");
								std::stringstream ss;
								ss << config.c_str() << " saved.";
								event_logs.add(ss.str(), color(167, 255, 255, 255));
							}
						}
					}
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button(("create"), ImVec2(175, 20))) {
						std::string config_file_name = entered_config_name;
						if (config_file_name.size() < 1) {
							config_file_name = ("settings");
						}
						config_system.create_config(config_file_name);

					}
					if (config.size() >= 1) {
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button(("delete"), ImVec2(175, 20))) {
							config_system.remove_config(config);

						}
					}

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("settings", ImVec2(279, 267), true); {
					ImGui::Combo("keybinds", &config_system.keybinds_selection, "edge jump\0aimbot key");

					if (config_system.keybinds_selection == 0) {
						ImGui::Hotkey("##edge jump key", &config_system.edge_jump_key, ImVec2(100, 20));
					}

					else if (config_system.keybinds_selection == 1) {
						ImGui::Hotkey("##aimbot key", &config_system.aim_key, ImVec2(100, 20));
					}
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("info", ImVec2(279, 543), true); {
					char buffer[UNLEN + 1];
					DWORD size;
					size = sizeof(buffer);
					GetUserName(buffer, &size);
					char title[UNLEN];
					char ch1[25] = "welcome, ";
					char *ch = strcat(ch1, buffer);

					ImGui::Text(ch);
					ImGui::Text("build: " __DATE__ " / " __TIME__);
					if (std::strstr(GetCommandLineA(), "-insecure")) {
						ImGui::Text("insecure mode!");
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			}
		}

		ImGui::PopFont();

		ImGui::End();
	}
}

void c_menu::run_popup() {
	ImGui::PushFont(font_menu);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	if (save_config) {
		bool done = false;
		ImGui::begin_popup("config saved.", 2000, &done);
		if (done)
			save_config = false;
	}

	if (load_config) {
		bool done = false;
		ImGui::begin_popup("config loaded.", 2000, &done);
		if (done)
			load_config = false;
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void c_menu::run_visuals_preview() {
	static std::vector<esp_info_s> info;
	static bool enabled = true;

	ImGui::SetNextWindowSize(ImVec2(235, 400));

	ImGui::Begin("ESP Preview", &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
		const auto cur_window = ImGui::GetCurrentWindow();
		const ImVec2 w_pos = cur_window->Pos;

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImColor c = ImColor(32, 114, 247);

		//title bar
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x - 20, p.y + 15), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 40), ImColor(30, 30, 39));

		//draw gradient bellow title bar
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 20, p.y + 14), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9); //góra, dół
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8); //lewo prawo
		//render cheat name
		ImGui::Text("visuals preview");


		if (config_system.player_box) {
			cur_window->DrawList->AddRect(ImVec2(w_pos.x + 40, w_pos.y + 60), ImVec2(w_pos.x + 200, w_pos.y + 360), ImGui::GetColorU32(ImGuiCol_Text));
		}

		if (config_system.player_health)
			cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 34, w_pos.y + 60), ImVec2(w_pos.x + 36, w_pos.y + 360), ImGui::GetColorU32(ImVec4(83 / 255.f, 200 / 255.f, 84 / 255.f, 255 / 255.f)));

		if (config_system.player_name)
			info.emplace_back(esp_info_s("name", color(255, 255, 255, 255), CENTER_UP));

		if (config_system.player_weapon)
			info.emplace_back(esp_info_s("awp", color(255, 255, 255, 255), CENTER_DOWN));

		if (config_system.player_flags_armor)
			info.emplace_back(esp_info_s("hk", color(255, 255, 255, 255), RIGHT));


		for (auto render : info) {
			const auto text_size = ImGui::CalcTextSize(render.f_name.c_str());

			auto pos = ImVec2(w_pos.x + 205, w_pos.y + 60);

			if (render.f_position == CENTER_DOWN) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y + 315 - text_size.y);
			}
			else if (render.f_position == CENTER_UP) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y - 5 - text_size.y);
			}

			cur_window->DrawList->AddText(pos, ImGui::GetColorU32(ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f)), render.f_name.c_str());
		}
	}
	ImGui::End();

	info.clear();
}
