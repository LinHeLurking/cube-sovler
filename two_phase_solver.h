#ifndef CUBE_SOLVER_TWO_PHASE_SOLVER_H
#define CUBE_SOLVER_TWO_PHASE_SOLVER_H

#include <cstdint>
#include <vector>
#include <gtest/gtest.h>

#include "cube_def.h"
#include "utility.h"

namespace Cube {

    //**********************************************************************
    // Declarations
    //**********************************************************************

    class TwoPhaseSolver {
    public:
        TwoPhaseSolver();

        std::vector<BasicMoveName> phaseOneSearch(const CubeStatus &start);

        std::vector<BasicMoveName> phaseTwoSearch(const CubeStatus &start);

    protected:
        static constexpr int UDSliceCnt = 495;

        uint32_t UDSliceHeuristic_[UDSliceCnt];
        uint32_t UDSliceMoveTable_[UDSliceCnt][18]; // 18 is the number of all basic moves.
        FRIEND_TEST(CubeTest, UDSliceMoveTable);


        ConstantFactory &constantFactory = ConstantFactory::getInstance();
        MoveFactory &moveFactory = MoveFactory::getInstance();

        void buildMoveTable();

        void buildHeuristic();

        void buildUdSliceMoveTable();

        void buildUDSliceHeuristic();
    };

    //**********************************************************************
    // Implementations
    //**********************************************************************

    std::vector<BasicMoveName> TwoPhaseSolver::phaseOneSearch(const CubeStatus &start) {
        return std::vector<BasicMoveName>();
    }

    std::vector<BasicMoveName> TwoPhaseSolver::phaseTwoSearch(const CubeStatus &start) {
        return std::vector<BasicMoveName>();
    }

    void TwoPhaseSolver::buildMoveTable() {
        buildUdSliceMoveTable();
    }

    void TwoPhaseSolver::buildUDSliceHeuristic() {

    }

    void TwoPhaseSolver::buildUdSliceMoveTable() {
        for (int i = 0; i < UDSliceCnt; ++i) {
            auto cur = moveFactory.getByUDSliceCoord(i);
            for (int face = 0; face <= BasicMoveName::R; ++face) {
                auto m = moveFactory.getMoveByEnum(static_cast<BasicMoveName>(face));
                for (int j = 0; j < 4; ++j) {
                    cur = m * cur;
                    if (j < 3) {
                        uint64_t co = cur.getUDSliceCoord();
                        UDSliceMoveTable_[i][3 * face + j] = co;
                    }
                }
            }
        }
    }

    void TwoPhaseSolver::buildHeuristic() {
        buildUDSliceHeuristic();
    }

    TwoPhaseSolver::TwoPhaseSolver() {
        buildMoveTable();
        buildHeuristic();
    }
}

#endif //CUBE_SOLVER_TWO_PHASE_SOLVER_H
