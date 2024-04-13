#ifndef SRC_OUTPUT_H
#define SRC_OUTPUT_H

#include <chrono>
#include <SFML/Graphics.hpp>

#include "Config.h"
#include "Control.h"

void print_help     ();
void set_text       (sf::Font* font, sf::Text* text, float x, float y);
void render_map     (sf::Uint8* pixels, Map_t* map, PlayerSet_t* PlayerSet);
void make_screenshot(sf::RenderWindow* window, const char* output_file);

void print_state_info(sf::RenderWindow* window, sf::Text* POS_Text, sf::Text* FPS_Text,
                      char* pos_string, size_t len_pos_string, char* fps_string, size_t len_fps_string,
                      std::chrono::_V2::steady_clock::time_point clock_begin,
                      std::chrono::_V2::steady_clock::time_point clock_end,
                      PlayerSet_t* PlayerSet, double* old_fps);

#endif // SRC_OUTPUT_H