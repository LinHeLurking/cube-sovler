
#include <iostream>
#include <gtest/gtest.h>

#include "utility.h"
#include "cube_def.h"
#include "two_phase_solver.h"

TEST(CubeTest, UDSLiceIndex) {
    using namespace Cube;
    auto &moveFactory = MoveFactory::getInstance();
    auto &CONSTANTS = ConstantFactory::getInstance();
    for (int i = 0; i < CONSTANTS.getBiCoef(12, 4); ++i) {
        auto status = moveFactory.getByUDSliceCoord(i);
        auto j = status.getUDSliceCoord();
        EXPECT_EQ(j, i);
    }
}

TEST(CubeTest, MoveOrd) {
    using namespace Cube;
    auto &moveFactory = MoveFactory::getInstance();
    auto id = moveFactory.Id_;
    for (int i = 0; i < BasicMoveName::R; ++i) {
        auto mName = static_cast<BasicMoveName>(i);
        auto m = moveFactory.getMoveByEnum(mName);
        auto res = m;
        res *= m;
        res *= m;
        res *= m;
        bool eq = res == id;
        EXPECT_EQ(eq, true) << i;
    }
}

namespace Cube {
    TEST(CubeTest, UDSliceMoveTable) {
        using namespace Cube;
        auto solver = TwoPhaseSolver();
        for (int i = 0; i < TwoPhaseSolver::UDSliceCnt; ++i) {
            auto status = solver.moveFactory.getByUDSliceCoord(i);
            for (int face = 0; face <= BasicMoveName::R; ++face) {
                auto m = solver.moveFactory.getMoveByEnum(static_cast<BasicMoveName>(face));
                auto co = (m * status).getUDSliceCoord();
                auto exp_co = solver.UDSliceMoveTable_[i][face * 3];
                EXPECT_EQ(co, exp_co);
            }
        }
    }
}
