#ifndef CUBE_SOLVER_TWO_PHASE_SOLVER_H
#define CUBE_SOLVER_TWO_PHASE_SOLVER_H

#include <vector>

#include "cube_def.h"
#include "utility.h"

namespace Cube {
    class TwoPhaseSolver {
    protected:
        ConstantFactory &constantFactory = ConstantFactory.getInstance();
        MoveFactory &moveFactory = MoveFactory.getInstance();

        void buildMoveTable() {

        }

        void buildIDAStarLowerBound() {

        }

    public:
        std::vector<BasicMoveName> phaseOneSearch(const IMove &start) {

        }

        std::vector<BasicMoveName> phaseTwoSearch(const IMove &start) {

        }
    };
}

#endif //CUBE_SOLVER_TWO_PHASE_SOLVER_H
