
#pragma once

#include "board.hpp"
#include "search_info.hpp"
#include "util.hpp"

#include <string>
#include <vector>

namespace UCIProtocol {

void send_info(const std::string &str);
void send_identity();
void process_go_command(const std::vector<std::string> &tokens,
                        const Board &board);
Board parse_position_command(const std::string &line);
void start_loop();

}; // namespace UCIProtocol
