#include "sap_select_pumpkin.h"

#include "bn_keypad.h"

#include "mj/mj_game_list.h"

#include "bn_regular_bg_items_sap_bg.h"
#include "bn_sprite_items_sap_dpad.h"
#include "bn_sprite_items_sap_pumpkin_1.h"
#include "bn_sprite_items_sap_pumpkin_2.h"
#include "bn_sprite_items_sap_pumpkin_3.h"
#include "bn_sprite_items_sap_pumpkin_4.h"
#include "bn_sprite_items_sap_pumpkin_5.h"
#include "bn_sprite_items_sap_pumpkin_6.h"
#include "bn_log.h"
#include "bn_keypad.h"
#include "bn_keypad.h"
#include <algorithm>
#include <random>

namespace
{
    constexpr bn::string_view code_credits[] = { "videogreg93" };
    constexpr bn::string_view graphics_credits[] = { "videogreg93" };

}

MJ_GAME_LIST_ADD(sap::select_pumpkin)
MJ_GAME_LIST_ADD_CODE_CREDITS(code_credits)
MJ_GAME_LIST_ADD_GRAPHICS_CREDITS(graphics_credits)
// MJ_GAME_LIST_ADD_MUSIC_CREDITS(music_credits)
// MJ_GAME_LIST_ADD_SFX_CREDITS(sfx_credits)

namespace sap
{
	bn::sprite_ptr random_pumpkin(bn::fixed_point pos, mj::game_data data) {
		switch (data.random.get_int(5)) {
			case 0:
				return bn::sprite_items::sap_pumpkin_2.create_sprite(pos);
				break;
			case 1:
				return bn::sprite_items::sap_pumpkin_3.create_sprite(pos);
				break;
			case 2:
				return bn::sprite_items::sap_pumpkin_4.create_sprite(pos);
				break;
			case 3:
				return bn::sprite_items::sap_pumpkin_5.create_sprite(pos);
				break;
			case 4:
				return bn::sprite_items::sap_pumpkin_6.create_sprite(pos);
				break;
			default:
				return bn::sprite_items::sap_pumpkin_2.create_sprite(pos);
				break;
			
		}
	}

select_pumpkin::select_pumpkin(int completed_games, const mj::game_data& data) :
    _bg(bn::regular_bg_items::sap_bg.create_bg((256 - 240) / 2, (256 - 160) / 2)),
	_dpad(bn::sprite_items::sap_dpad.create_sprite(0,0)),
    _total_frames(play_jingle(mj::game_jingle_type::EXELOTL01, completed_games, data)),
    _difficulty(recommended_difficulty_level(completed_games, data))
{
	_positions.push_back(bn::fixed_point(0, -32));
	_positions.push_back(bn::fixed_point(0, 32));
	_positions.push_back(bn::fixed_point(-32,0));
	_positions.push_back(bn::fixed_point(32, 0));
	_winner = data.random.get_int(4);
	for (int i = 0; i < _positions.size(); i++) {
		bn::fixed_point pos = _positions[i];
		if (i == _winner) {
			_pumpkin_sprites.push_back(bn::sprite_items::sap_pumpkin_1.create_sprite(pos));
		} else {
			_pumpkin_sprites.push_back(random_pumpkin(pos, data));
		}
	}
}

void select_pumpkin::fade_in([[maybe_unused]] const mj::game_data& data)
{
}

mj::game_result select_pumpkin::play(const mj::game_data& data)
{
    _elapsed_frames++;
    mj::game_result result;
    result.exit = data.pending_frames == 0;

    if(! _victory && ! _defeat)
    {
		int button_pressed = -1;
		if (bn::keypad::up_pressed()) {
			button_pressed = 0;
		} else if (bn::keypad::down_pressed()) {
			button_pressed = 1;
		} else if (bn::keypad::left_pressed()) {
			button_pressed = 2;
		} else if (bn::keypad::right_pressed()) {
			button_pressed = 3;
		}
		if (button_pressed != -1) {
			if (button_pressed == _winner) {
				result.remove_title = true;
                _victory = true;
                _defeat = false;
			} else {
				result.remove_title = true;
                _victory = false;
                _defeat = true;
			}
		}

    }
    else
    {
        if(_show_result_frames)
        {
			if (_victory) {
				for (int i = 0; i < _pumpkin_sprites.size(); i++) {
					bn::fixed new_rotation = _pumpkin_sprites[i].rotation_angle() + 15;
					if (new_rotation > 360) {
						new_rotation -= 360;
					}
					_pumpkin_sprites[i].set_rotation_angle(new_rotation);
				}
			} else {
				for (int i = 0; i < _pumpkin_sprites.size(); i++) {
					_pumpkin_sprites[i].set_rotation_angle(180);
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

void select_pumpkin::fade_out([[maybe_unused]] const mj::game_data& data)
{
}

}
