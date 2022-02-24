#include <fstream>
#include <Eigen/Core>
#include <torch/types.h>
#include <torch/serialize.h>
#include "ProximalPolicyOptimization.h"
#include "Models.h"
#include "TestEnvironment.h"

int test() {

    // Random engine.
    std::random_device rd;
    std::mt19937 re(rd());
    std::uniform_int_distribution<> dist(-5, 5);

    // Environment.
    double x = double(dist(re)); // goal x pos
    double y = double(dist(re)); // goal y pos
    TestEnvironment env(x, y);

    // Model.
    uint n_in = 4;
    uint n_out = 2;
    double std = 1e-2;

    ActorCritic ac(n_in, n_out, std);
    ac->to(torch::kF64);
    ac->normal(0., std);
    ac->eval();
    torch::load(ac, "best_model.pt");

    // Training loop.
    uint n_iter = 10000;

    // Output.
    std::ofstream out;
    out.open("../data/data_test.csv");

    // episode, agent_x, agent_y, goal_x, goal_y, STATUS=(PLAYING, WON, LOST, RESETTING)
    out << 1 << ", " << env.pos_(0) << ", " << env.pos_(1) << ", " << env.goal_(0) << ", " << env.goal_(1) << ", "
        << RESETTING << "\n";

    // Counter.
    uint c = 0;

    for (uint i = 0; i < n_iter; i++) {
        // Play.
        auto av = ac->forward(env.State());
        auto action = std::get<0>(av);

        double x_act = action[0][0].item<double>();
        double y_act = action[0][1].item<double>();
        auto sd = env.Act(x_act, y_act);

        // Check for done state.
        auto done = std::get<2>(sd);

        // episode, agent_x, agent_y, goal_x, goal_y, AGENT=(PLAYING, WON, LOST, RESETTING)
        out << 1 << ", " << env.pos_(0) << ", " << env.pos_(1) << ", " << env.goal_(0) << ", " << env.goal_(1) << ", "
            << std::get<1>(sd) << "\n";

        if (done[0][0].item<double>() == 1.) {
            // Set new goal.
            double x_new = double(dist(re));
            double y_new = double(dist(re));
            env.SetGoal(x_new, y_new);

            // Reset the position of the agent.
            env.Reset();

            // episode, agent_x, agent_y, goal_x, goal_y, STATUS=(PLAYING, WON, LOST, RESETTING)
            out << 1 << ", " << env.pos_(0) << ", " << env.pos_(1) << ", " << env.goal_(0) << ", " << env.goal_(1)
                << ", " << RESETTING << "\n";
        }
    }

    out.close();

    return 0;
}
