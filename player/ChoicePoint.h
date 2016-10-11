//
// Created by baj on 10/4/16.
//

#ifndef KEEPAWAY_PLAYER_CHOICEPOINT_H
#define KEEPAWAY_PLAYER_CHOICEPOINT_H


#include <string>
#include <vector>
#include <cstring>
#include <functional>
#include "HierarchicalFSM.h"

namespace fsm {

template<class T>
class ChoicePoint {
public:
  ChoicePoint(const std::string name, const std::vector<T> &c) :
      name(name),
      choices(c) {

  }

  T choose() {
    Log.logWithTime(101, "ChoicePoint::choose point name: %s", name.c_str());
    auto jc = LinearSarsaLearner::ins().step((int) choices.size());
    auto i = jc[LinearSarsaLearner::ins().agentIdx];

    if (jc.size()) {
      Log.logWithTime(101, "ChoicePoint::choose my choice (agent %d): %s", LinearSarsaLearner::ins().agentIdx,
                      to_string(choices[i]).c_str());
    }
    return choices[i];
  }

private:
  std::string name;
  std::vector<T> choices;
};

}

#endif //KEEPAWAY_PLAYER_CHOICEPOINT_H
