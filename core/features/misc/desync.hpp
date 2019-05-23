#pragma once

#include "../../../source-sdk/structs/animstate.hpp"
#include "../../../dependencies/common_includes.hpp"

class c_desync {
public:
	void run(c_usercmd* user_cmd, bool& send_packet);
private:
	bool is_enabled(c_usercmd* user_cmd);
	bool is_firing(c_usercmd* user_cmd);
	bool can_fire();
	float get_max_desync_delta();
};

extern c_desync desync;
