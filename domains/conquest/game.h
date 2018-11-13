// Copyright 2018 The Darwin Neuroevolution Framework Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "board.h"

#include <core/properties.h>

#include <random>
#include <string>
#include <vector>
using namespace std;

namespace conquest {

class Player;

//! Conquest domain configuration
struct Config : public core::PropertySet {
  PROPERTY(eval_games, int, 5, "Number of evaluation games");
  PROPERTY(calibration_games, int, 100, "Number of calibration games");

  PROPERTY(board,
           BoardConfiguration,
           BoardConfiguration::Hexagon,
           "Board configuration name");

  PROPERTY(max_steps, int, 2500, "If no one wins before max_steps, the game is a tie");
  PROPERTY(rematches, bool, true, "Play rematches?");

  PROPERTY(win_points, float, 1.0f, "Points for a win");
  PROPERTY(lose_points, float, 0.0f, "Points for a lost game");
  PROPERTY(draw_points, float, 0.4f, "Points for a draw");

  PROPERTY(int_unit_scale, float, 10.0f, "A scaling factor to display units as integers");

  // units parameters
  PROPERTY(initial_units, float, 0.1f, "Initial units");
  PROPERTY(production_cap, float, 1.05f, "Production cap per node");
  PROPERTY(production_step, float, 0.005f, "Production step");

  // order (attack) parameters
  PROPERTY(deploy_min,
           float,
           1.0f / int_unit_scale,
           "Minimum units required to deploy an attack");
  PROPERTY(deploy_resolution, float, deploy_min, "Deployment resolution");
  PROPERTY(units_speed, float, 2.0f, "Units move speed");
  PROPERTY(deploy_percent, float, 0.99f, "What % of units are deployed? (0..1]");
};

extern Config g_config;

class Game {
 public:
  enum class State { None, InProgress, BlueWins, RedWins, Draw };

  struct Stats {
    // abs(total units, including in-flight deployments)
    float blue_units;
    float red_units;

    // controlled nodes
    int blue_nodes;
    int red_nodes;

    // in flight deployments
    int blue_orders;
    int red_orders;
  };

  struct Deployment {
    float size;
    float position;
  };

 public:
  Game(int max_steps, const Board* board);

  Game(const Game&) = delete;
  Game& operator=(const Game&) = delete;

  void newGame(Player* blue_player, Player* red_player);
  void rematch();
  bool gameStep();
  void order(int arc_index);

  const Board* board() const { return board_; }
  const Player* bluePlayer() const { return blue_player_; }
  const Player* redPlayer() const { return red_player_; }

  State state() const { return state_; }
  int currentStep() const { return step_; }
  bool finished() const;

  Stats computeStats() const;

  float nodeUnits(int index) const;
  const Deployment& deployment(int index) const;

  const vector<float>& nodeUnits() const { return node_units_; }
  const vector<Deployment>& deployments() const { return deployments_; }

 private:
  void resetGame();

 private:
  State state_ = State::None;
  const Board* const board_ = nullptr;

  vector<float> node_units_;
  vector<Deployment> deployments_;

  int step_ = -1;
  const int max_steps_ = -1;

  Player* blue_player_ = nullptr;
  Player* red_player_ = nullptr;

  int blue_start_node_ = -1;
  int red_start_node_ = -1;

  default_random_engine rnd_{ random_device{}() };
};

}  // namespace conquest