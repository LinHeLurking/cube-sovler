#ifndef CUBE_SOLVER_UTILITY_H
#define CUBE_SOLVER_UTILITY_H

#include <cstdint>

namespace Cube {
    enum BasicMoveName {
        U, D, F, B, L, R,
        Ui, Di, Fi, Bi, Li, Ri
    };

    enum Color {
        Green, Yellow, Red, Blue, Orange, White
    };

    enum BlockPos {
        URF = 0, UFL, ULB, UBR, DFR, DLF, DBL, DRB, UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
    };

    class ConstantFactory {
    private:
        // Private constructor
        ConstantFactory() {
            factorial_[0] = 1;
            for (int i = 1; i < IDX_BOUND; ++i) {
                factorial_[i] = factorial_[i - 1] * i;
            }

            for (int n = 0; n < IDX_BOUND; ++n) {
                for (int k = 0; k <= n; ++k) {
                    binomial_[n][k] = factorial_[n] / (factorial_[k] * factorial_[n - k]);
                }
                for (int k = n + 1; k < IDX_BOUND; ++k) {
                    binomial_[n][k] = 0;
                }
            }
        }

        // Member variables

        int64_t binomial_[IDX_BOUND][IDX_BOUND];
        int64_t factorial_[IDX_BOUND];

    public:
        constexpr unsigned IDX_BOUND = 18;

        // Prevent instantiation
        ConstantFactory(ConstantFactory &) = delete;

        ConstantFactory(ConstantFactory &&) = delete;

        static ConstantFactory &getInstance() {
            static ConstantFactory INSTANCE = ConstantFactory();
            return INSTANCE;
        }

        int64_t getFactorial(int k) const {
            return factorial_[k];
        }

        int64_t getBinomialCoefficient(int n, int k) {
            return binomial_[n][k];
        }
    };
}

#endif //CUBE_SOLVER_UTILITY_H
