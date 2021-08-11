#ifndef CUBE_SOLVER_TWO_PHASE_SOLVER_H
#define CUBE_SOLVER_TWO_PHASE_SOLVER_H

#include <cstdint>
#include <vector>

#include "cube_def.h"
#include "utility.h"

namespace Cube {

    //**********************************************************************
    // Declarations
    //**********************************************************************

    class TwoPhaseSolver {
    public:
        TwoPhaseSolver() = default;

        std::vector<BasicMoveName> phaseOneSearch(const CubeStatus &start);

        std::vector<BasicMoveName> phaseTwoSearch(const CubeStatus &start);

    protected:
        static constexpr UDSliceCnt = 495;

        uint32_t UDSliceHeuristic_[UDSliceCnt];
        uint32_t UDSliceMoveTable_[UDSliceCnt][18]; // 18 is the number of all basic moves.


        ConstantFactory &constantFactory = ConstantFactory.getInstance();
        MoveFactory &moveFactory = MoveFactory.getInstance();

        void buildMoveTable();

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

    }

    void TwoPhaseSolver::buildUDSliceHeuristic() {

    }
}

#endif //CUBE_SOLVER_TWO_PHASE_SOLVER_H
