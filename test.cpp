
#include <iostream>
#include <gtest/gtest.h>

#include "utility.h"
#include "cube_def.h"

TEST(CubeTest, TryRun) {
    std::cout << "Hello World" << std::endl;
}

TEST(CubeTest, RandomCube) {
    auto &moveFactory = Cube::MoveFactory::getInstance();
    auto res = moveFactory.genRandomCube();
    for (int i = Cube::BlockPos::URF; i <= Cube::BlockPos::BR; ++i) {
        std::cout << res.move[i].pos_ << " " << static_cast<int>(res.move[i].ori_) << std::endl;
    }
}
