#ifndef GKF_MY_GAME_H
#define GKF_MY_GAME_H

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_tiles_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_rect.h"

#include "mj/mj_game.h"

namespace gkf
{

class my_game : public mj::game
{

public:
    my_game(int completed_games, const mj::game_data& data);

    [[nodiscard]] bn::string<16> title() const final
    {
        return "Get the Candy!";
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
    bn::vector<bn::sprite_ptr, 16> _candy_sprites;
    bn::vector<bn::sprite_ptr, 2> _pumpkin_sprites;
    bn::sprite_ptr _bucket;
    bn::fixed_rect _bucket_collider;
    mj::difficulty_level _difficulty;
    int _total_frames;
    int _show_result_frames = 60;
    bool _victory = false;
    bool _defeat = false;
    int _candy_speed = 2;
    int _candy_speed_hard = 3;
    int _elapsed_frames = 0;
    int _bucket_speed = 2;
    int _total_candy_caught = 0;
    int _total_candy_needed;
};

}

#endif
