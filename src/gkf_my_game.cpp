#include "gkf_my_game.h"

#include "bn_keypad.h"

#include "mj/mj_game_list.h"

#include "bn_regular_bg_items_gkf_bg.h"
#include "bn_regular_bg_items_tmg_you_lose.h"
#include "bn_regular_bg_items_tmg_you_win.h"
#include "bn_sprite_items_gkf_bucket.h"
#include "bn_sprite_items_gkf_candy_1.h"
#include "bn_sprite_items_gkf_candy_2.h"
#include "bn_sprite_items_gkf_candy_3.h"
#include "bn_sprite_items_gkf_pumpkin_happy.h"
#include "bn_sprite_items_gkf_pumpkin_sad.h"
#include "bn_log.h"
#include "bn_keypad.h"
#include "bn_keypad.h"

namespace
{
    constexpr bn::string_view code_credits[] = { "videogreg93" };
    constexpr bn::string_view graphics_credits[] = { "videogreg93" };

}

MJ_GAME_LIST_ADD(gkf::my_game)
MJ_GAME_LIST_ADD_CODE_CREDITS(code_credits)
MJ_GAME_LIST_ADD_GRAPHICS_CREDITS(graphics_credits)
// MJ_GAME_LIST_ADD_MUSIC_CREDITS(music_credits)
// MJ_GAME_LIST_ADD_SFX_CREDITS(sfx_credits)

namespace gkf
{

my_game::my_game(int completed_games, const mj::game_data& data) :
    _bg(bn::regular_bg_items::gkf_bg.create_bg((256 - 240) / 2, (256 - 160) / 2)),
    _total_frames(play_jingle(mj::game_jingle_type::EXELOTL01, completed_games, data)),
    _bucket(bn::sprite_items::gkf_bucket.create_sprite(0,60)),
    _difficulty(recommended_difficulty_level(completed_games, data))
{
    _bucket_collider.set_position(12,60 + 13);
    _bucket_collider.set_dimensions(40,20);
    switch (recommended_difficulty_level(completed_games, data)) {
        case mj::difficulty_level::EASY:
            _total_candy_needed = 1;
            break;
        case mj::difficulty_level::NORMAL:
            _total_candy_needed = 2;
            break;
        case mj::difficulty_level::HARD:
            _total_candy_needed = 2;
            break;
        default:
            _total_candy_needed = 1;
            break;
    }

}

void my_game::fade_in([[maybe_unused]] const mj::game_data& data)
{
}

bn::sprite_ptr create_candy(const mj::game_data& data, int x, int y) {
    int random = data.random.get_int(3);
    switch (random) {
        case 0:
            return bn::sprite_items::gkf_candy_1.create_sprite(x,y);
            break;
        case 1:
            return bn::sprite_items::gkf_candy_2.create_sprite(x,y);
            break;
        case 2:
            return bn::sprite_items::gkf_candy_3.create_sprite(x,y);
            break;
        default:
            return bn::sprite_items::gkf_candy_1.create_sprite(x,y);
            break;
    }
}

mj::game_result my_game::play(const mj::game_data& data)
{
    _elapsed_frames++;
    mj::game_result result;
    result.exit = data.pending_frames == 0;

    if(! _victory && ! _defeat)
    {
        // Update bucket collider
        _bucket_collider.set_position(_bucket.x() + 9, _bucket.y() + 11);
        _bucket_collider.set_dimensions(49,20);
        if (_elapsed_frames >= 60) {
            _elapsed_frames = 0;
            int offset = data.random.get_int(5);
            int candy_x = (offset * 48) - 95;
            _candy_sprites.push_back(create_candy(data, candy_x, -80));
        }
        // Move Player
        if(bn::keypad::left_held())
        {
            _bucket.set_x(_bucket.x() - _bucket_speed);
        }
        else if(bn::keypad::right_held())
        {
            _bucket.set_x(_bucket.x() + _bucket_speed);
        }
        // Move candy
        bn::vector<bn::sprite_ptr, 16> candy_to_remove;
        for(int index = 0; index < _candy_sprites.size(); ++index) {
            if (_difficulty == mj::difficulty_level::HARD) {
                _candy_sprites[index].set_y(_candy_sprites[index].y() + _candy_speed_hard);
            } else {
                _candy_sprites[index].set_y(_candy_sprites[index].y() + _candy_speed);
            }
            // check bucket collision
            bn::fixed_rect candy_collider = bn::fixed_rect(
                    _candy_sprites[index].x(),
                    _candy_sprites[index].y(),
                    32,
                    32
                    );
            if (_bucket_collider.intersects(candy_collider)) {
                candy_to_remove.push_back(_candy_sprites[index]);
                _total_candy_caught++;
            } else if (_candy_sprites[index].y() > 80) {
                // Candy offscreen, you lose
                result.remove_title = true;
                _victory = false;
                _defeat = true;
                _pumpkin_sprites.push_back(bn::sprite_items::gkf_pumpkin_sad.create_sprite(0,0));
            }
        }
        // Remove candy off screen
        _candy_sprites.erase(std::remove_if(_candy_sprites.begin(), _candy_sprites.end(), [&candy_to_remove](bn::sprite_ptr s) {
            return s.y() > 80 || std::find(candy_to_remove.begin(), candy_to_remove.end(), s) != candy_to_remove.end();
        }), _candy_sprites.end());

        if (_total_candy_caught >= _total_candy_needed) {
            // You win
            result.remove_title = true;
            _victory = true;
            _pumpkin_sprites.push_back(bn::sprite_items::gkf_pumpkin_happy.create_sprite(-60,0));
            _pumpkin_sprites.push_back(bn::sprite_items::gkf_pumpkin_happy.create_sprite(60,0));
            _pumpkin_sprites[1].set_rotation_angle(315);
            _pumpkin_sprites[0].set_rotation_angle(45);
        }
    }
    else
    {
        if(_show_result_frames)
        {
            // Animate pumpkins
            if (_victory && _show_result_frames % 10 == 0) {
                if (_pumpkin_sprites[1].rotation_angle() == 315) {
                    _pumpkin_sprites[1].set_rotation_angle(45);
                    _pumpkin_sprites[0].set_rotation_angle(315);
                } else {
                    _pumpkin_sprites[1].set_rotation_angle(315);
                    _pumpkin_sprites[0].set_rotation_angle(45);
                }
            } else if (_defeat) {
                if (_show_result_frames > 30) {
                    _pumpkin_sprites[0].set_scale(_pumpkin_sprites[0].horizontal_scale() + 0.025);
                } else {
                    _pumpkin_sprites[0].set_scale(_pumpkin_sprites[0].horizontal_scale() - 0.025);
                }
            }
            --_show_result_frames;
        }
        else
        {
            result.exit = true;
        }
    }

    return result;
}

void my_game::fade_out([[maybe_unused]] const mj::game_data& data)
{
}

}
