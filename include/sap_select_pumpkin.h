#ifndef SAP_SELECT_PUMPKIN_H
#define SAP_SELECT_PUMPKIN_H

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_rect.h"
#include "bn_fixed_point.h"

#include "mj/mj_game.h"

namespace sap
{

class select_pumpkin : public mj::game
{

public:
    select_pumpkin(int completed_games, const mj::game_data& data);

    [[nodiscard]] bn::string<16> title() const final
    {
        return "Select Happy!";
    }

    [[nodiscard]] int total_frames() const final
    {
        return _total_frames;
    }

    void fade_in(const mj::game_data& data) final;

    [[nodiscard]] mj::game_result play(const mj::game_data& data) final;

    [[nodiscard]] bool victory() const final
    {
        return _victory;
    }

    void fade_out(const mj::game_data& data) final;

private:
    bn::regular_bg_ptr _bg;
	bn::sprite_ptr _dpad;
    mj::difficulty_level _difficulty;
	bn::fixed_point _up;
	bn::fixed_point _down;
	bn::fixed_point _left;
	bn::fixed_point _right;
	bn::vector<bn::fixed_point, 4> _positions;
	bn::vector<bn::sprite_ptr, 6> _all_pumpkin_sprites;
	bn::vector<bn::sprite_ptr, 4> _pumpkin_sprites;
	int _winner;
    int _total_frames;
    int _show_result_frames = 60;
    bool _victory = false;
    bool _defeat = false;
    int _elapsed_frames = 0;
};

}

#endif
