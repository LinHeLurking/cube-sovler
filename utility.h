#ifndef CUBE_SOLVER_UTILITY_H
#define CUBE_SOLVER_UTILITY_H

#include <limits>
#include <random>
#include <cstdint>

namespace Cube {
    enum BasicMoveName {
        U, D, F, B, L, R
    };

    enum Color {
        Green, Yellow, Red, Blue, Orange, White
    };

    enum BlockPos {
        URF = 0, UFL, ULB, UBR, DFR, DLF, DBL, DRB, UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
    };

    class ConstantFactory {
    public:
        static ConstantFactory &getInstance() {
            static ConstantFactory INSTANCE;
            return INSTANCE;
        }

        static constexpr unsigned IDX_BOUND = 18;

        // Prevent instantiation
        ConstantFactory(const ConstantFactory &) = delete;

        void operator=(const ConstantFactory &) = delete;

        int64_t getFactorial(int k) const {
            return factorial_[k];
        }

        int64_t getBiCoef(int n, int k) {
            return binomial_[n][k];
        }

    protected:
        // Member variables

        int64_t binomial_[IDX_BOUND][IDX_BOUND];
        int64_t factorial_[IDX_BOUND];

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
    };

    class RandomFactory {
    public:
        static inline RandomFactory &getInstance() {
            static RandomFactory INSTANCE;
            return INSTANCE;
        }

        RandomFactory(const RandomFactory &) = delete;

        void operator=(const RandomFactory &) = delete;

        std::mt19937 generator_;
    protected:
        std::random_device device_{};


        RandomFactory() {
            generator_ = std::mt19937(device_());
        }
    };
}

#endif //CUBE_SOLVER_UTILITY_H
