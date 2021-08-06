#ifndef CUBE_SOLVER_CUBE_DEF_H
#define CUBE_SOLVER_CUBE_DEF_H

#include <string>
#include <array>
#include <cstdint>
#include <algorithm>
#include <random>
#include <iostream>

#include "utility.h"

namespace Cube {

    class MoveResult {
    public:
        BlockPos pos_;
        int8_t ori_;

        MoveResult() = default;

        MoveResult(BlockPos pos, int8_t ori) : pos_(pos), ori_(ori) {}

        MoveResult(int pos, int8_t ori) : pos_(static_cast<BlockPos>(pos)), ori_(ori) {}
    };

    class CubeStatus {
    public:
        CubeStatus() = default;

        MoveResult move[20];

        // Position is replaced-by. Orientation is in-place.
        MoveResult operator()(BlockPos pos) {
            return move[pos];
        }

        int getConorOriCoord() {
            int co = 0;
            // Summation of conor orientations is divided by 3.
            for (int i = BlockPos::URF; i < BlockPos::DRB; ++i) {
                co = 3 * co + move[i].ori_;
            }
            return co;
        }

        int getEdgeOriCoord() {
            int co = 0;
            // Summation of edge orientations is divided by 2.
            for (int i = BlockPos::UR; i < BlockPos::BR; ++i) {
                co = 2 * co + move[i].ori_;
            }
            return co;
        }

        int getConorPermCoord() {
            int co = 0;
            for (int i = BlockPos::DRB; i > BlockPos::URF; --i) {
                int s = 0;
                for (int j = i - 1; j >= BlockPos::URF; --j) {
                    if (move[j].pos_ > move[i].pos_) {
                        s += 1;
                    }
                }
                // (i + 1) is the order of this conor.
                co = (co + s) * (i + 1);
            }
            return co;
        }

        int getEdgePermCoord() {
            int co = 0;

            for (int i = BlockPos::BR; i > BlockPos::UR; --i) {
                int s = 0;
                for (int j = i - 1; j >= BlockPos::UR; --j) {
                    if (move[j].pos_ > move[i].pos_) {
                        s += 1;
                    }
                }
                // (i + 1 - BlockPos::UR) is the order of this edge.
                co = (co + s) * (i + 1 - BlockPos::UR);
            }
            return co;
        }

        int getUDSliceCoord() {
            bool occupied[12];
            std::fill(occupied, occupied + 12, false);
            for (int i = BlockPos::UR; i <= BlockPos::BR; ++i) {
                if (move[i].pos_ >= BlockPos::FR) {
                    occupied[i - BlockPos::UR] = true;
                }
            }
            int s = 0, k = 3, n = 11;
            ConstantFactory &constantFactory = ConstantFactory::getInstance();
            while (k >= 0) {
                if (occupied[n]) {
                    k -= 1;
                }
                s += static_cast<int>(constantFactory.getBinomialCoefficient(n, k));
                n -= 1;
            }
            return s;
        }

        /*
         * In phase 2, we only use the subgroup <U_, D_, L_^2, R_^2, F_^2, B_^2>
         * */

        int getPhase2EdgePermCoord() {
            int co = 0;
            for (int i = BlockPos::DB; i > BlockPos::UR; --i) {
                int s = 0;
                for (int j = i - 1; j >= BlockPos::UR; --j) {
                    if (move[j].pos_ > move[i].pos_) {
                        s += 1;
                    }
                }
                // (i + 1 - BlockPos::UR) is the order of this edge.
                co = (s + co) * (i + 1 - BlockPos::UR);
            }
            return co;
        }

        int getUDSliceSortedCoord() {
            BlockPos arr[4];
            int j;
            for (int i = BlockPos::UR; i <= BlockPos::BR; ++i) {
                BlockPos e = move[i].pos_;
                if (e == BlockPos::FR || e == BlockPos::FL || e == BlockPos::BL || e == BlockPos::BR) {
                    arr[j] = e;
                    j += 1;
                }
            }

            int co = 0;
            for (j = 3; j >= 1; --j) {
                int s = 0;
                for (int k = j - 1; k >= 0; --k) {
                    if (arr[k] > arr[j]) {
                        s += 1;
                    }
                }
                co = (co + s) * j;
            }
            co = co + 24 * getUDSliceCoord();
            return co;
        }


        // Composition Operators
        CubeStatus &operator*=(const CubeStatus &other) {
#ifdef DEBUG
//            std::cout << "operator*=" << std::endl;
#endif
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                BlockPos otherPos = other.move[i].pos_;
                int8_t otherOri = other.move[i].ori_;
                this->move[i].pos_ = this->move[otherPos].pos_;
                this->move[i].ori_ = this->move[otherPos].ori_ + otherOri;
                if (i <= BlockPos::DRB && this->move[i].ori_ >= 3) {
                    this->move[i].ori_ -= 3;
                } else if (i >= BlockPos::UR && this->move[i].ori_ >= 2) {
                    this->move[i].ori_ -= 2;
                }
            }
            return *this;
        }

        CubeStatus operator*(const CubeStatus &other) const {
            auto ret = CubeStatus(*this);
            ret *= other;
            return ret;
        }

        CubeStatus inverse() const {
            CubeStatus ret;
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                ret.move[this->move[i].pos_].pos_ = static_cast<BlockPos>(i);
                ret.move[this->move[i].pos_].ori_ = static_cast<int8_t>(3) - this->move[i].ori_;
                if (ret.move[i].ori_ >= 3) {
                    ret.move[i].ori_ -= 3;
                }
            }
            return ret;
        }
    };


    class MoveFactory {
    public:
        // Prevent instantiation
        MoveFactory(MoveFactory &) = delete;

        void operator=(MoveFactory &) = delete;

        static MoveFactory &getInstance() {
            static MoveFactory INSTANCE;
            return INSTANCE;
        }

        inline CubeStatus &getMoveByEnum(BasicMoveName m) {
            switch (m) {
                case BasicMoveName::U:
                    return U_;
                case BasicMoveName::D:
                    return D_;
                case BasicMoveName::L:
                    return L_;
                case BasicMoveName::R:
                    return R_;
                case BasicMoveName::F:
                    return F_;
                case BasicMoveName::B:
                    return B_;
                default:
                    throw std::runtime_error(std::string(__func__) + "Unexpected move name");
                    break;
            }
        }

        inline CubeStatus genRandomCube(uint64_t randomStep = 50) {
            auto &randomFactory = RandomFactory::getInstance();
            CubeStatus cur = Id_;
            std::uniform_int_distribution<> dist;
            while (randomStep-- > 0) {
#ifdef DEBUG
                std::cout << "randomStep: " << randomStep << std::endl;
#endif
                auto moveEnum = static_cast<BasicMoveName>(dist(randomFactory.generator_) % 6);
                CubeStatus move = getMoveByEnum(moveEnum);
                cur *= move;
            }
            return cur;
        }

        // Identity move
        CubeStatus Id_;

        // Basic moves
        CubeStatus L_, R_, U_, D_, F_, B_;
        //        CubeStatus Li, Ri, Ui, Di, Fi, Bi;

        CubeStatus Sym_[48];
    private:
        // Private constructors
        MoveFactory() {
            // Init id move
            setAsId(Id_);

            // Init basic moves
            setAsU(U_);
            setAsD(D_);
            setAsL(L_);
            setAsR(R_);
            setAsF(F_);
            setAsB(B_);

            // Init inverse of basic moves
//            Li = L_.inverse();
//            Ri = R_.inverse();
//            Ui = U_.inverse();
//            Di = D_.inverse();
//            Fi = F_.inverse();
//            Bi = B_.inverse();

            // Init basic symmetric moves
            setAsS_URF3(S_URF3);
            setAsS_F2(S_F2);
            setAsS_U4(S_U4);
            setAsS_LR2(S_LR2);

            // Init all symmetric moves
            for (int a = 0; a < 3; ++a) {
                for (int b = 0; b < 2; ++b) {
                    for (int c = 0; c < 4; ++c) {
                        for (int d = 0; d < 2; ++d) {
                            int idx = 16 * a + 8 * b + 2 * c + d;
                            CubeStatus S = Id_;
                            for (int i = 0; i < a; ++i) {
                                S *= S_URF3;
                            }
                            for (int i = 0; i < b; ++i) {
                                S *= S_F2;
                            }
                            for (int i = 0; i < c; ++i) {
                                S *= S_U4;
                            }
                            for (int i = 0; i < d; ++i) {
                                S *= S_LR2;
                            }
                            Sym_[idx] = S;
                        }
                    }
                }
            }
        }

    protected:
        // Basic symmetric moves
        CubeStatus S_URF3, S_F2, S_U4, S_LR2;

        static inline void setAsId(CubeStatus &m) {
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                m.move[i].pos_ = static_cast<BlockPos>(i);
                m.move[i].ori_ = 0;
            }
        }

        static inline void setAsL(CubeStatus &m) {

            m.move[URF] = MoveResult(URF, 0);
            m.move[UFL] = MoveResult(ULB, 1);
            m.move[ULB] = MoveResult(DBL, 2);
            m.move[UBR] = MoveResult(UBR, 0);
            m.move[DFR] = MoveResult(DFR, 0);
            m.move[DLF] = MoveResult(UFL, 2);
            m.move[DBL] = MoveResult(DLF, 1);
            m.move[DRB] = MoveResult(DRB, 0);

            m.move[UR] = MoveResult(UR, 0);
            m.move[UF] = MoveResult(UF, 0);
            m.move[UL] = MoveResult(BL, 0);
            m.move[UB] = MoveResult(UB, 0);
            m.move[DR] = MoveResult(DR, 0);
            m.move[DF] = MoveResult(DF, 0);
            m.move[DL] = MoveResult(FL, 0);
            m.move[DB] = MoveResult(DB, 0);
            m.move[FR] = MoveResult(FR, 0);
            m.move[FL] = MoveResult(UL, 0);
            m.move[BL] = MoveResult(DL, 0);
            m.move[BR] = MoveResult(BR, 0);
        }

        static inline void setAsR(CubeStatus &m) {
            m.move[URF] = MoveResult(DFR, 2);
            m.move[UFL] = MoveResult(UFL, 0);
            m.move[ULB] = MoveResult(ULB, 0);
            m.move[UBR] = MoveResult(URF, 1);
            m.move[DFR] = MoveResult(DRB, 1);
            m.move[DLF] = MoveResult(DLF, 0);
            m.move[DBL] = MoveResult(DBL, 0);
            m.move[DRB] = MoveResult(UBR, 2);

            m.move[UR] = MoveResult(FR, 0);
            m.move[UF] = MoveResult(UF, 0);
            m.move[UL] = MoveResult(UL, 0);
            m.move[UB] = MoveResult(UB, 0);
            m.move[DR] = MoveResult(BR, 0);
            m.move[DF] = MoveResult(DF, 0);
            m.move[DL] = MoveResult(DL, 0);
            m.move[DB] = MoveResult(DB, 0);
            m.move[FR] = MoveResult(DR, 0);
            m.move[FL] = MoveResult(FL, 0);
            m.move[BL] = MoveResult(BL, 0);
            m.move[BR] = MoveResult(UR, 0);

        }

        static inline void setAsU(CubeStatus &m) {
            m.move[URF] = MoveResult(UBR, 0);
            m.move[UFL] = MoveResult(URF, 0);
            m.move[ULB] = MoveResult(UFL, 0);
            m.move[UBR] = MoveResult(ULB, 0);
            m.move[DFR] = MoveResult(DFR, 0);
            m.move[DLF] = MoveResult(DLF, 0);
            m.move[DBL] = MoveResult(DBL, 0);
            m.move[DRB] = MoveResult(DRB, 0);


            m.move[UR] = MoveResult(UB, 0);
            m.move[UF] = MoveResult(UR, 0);
            m.move[UL] = MoveResult(UF, 0);
            m.move[UB] = MoveResult(UL, 0);
            m.move[DR] = MoveResult(DR, 0);
            m.move[DF] = MoveResult(DF, 0);
            m.move[DL] = MoveResult(DL, 0);
            m.move[DB] = MoveResult(DB, 0);
            m.move[FR] = MoveResult(FR, 0);
            m.move[FL] = MoveResult(FL, 0);
            m.move[BL] = MoveResult(BL, 0);
            m.move[BR] = MoveResult(BR, 0);
        }

        static inline void setAsD(CubeStatus &m) {

            m.move[URF] = MoveResult(URF, 0);
            m.move[UFL] = MoveResult(UFL, 0);
            m.move[ULB] = MoveResult(ULB, 0);
            m.move[UBR] = MoveResult(UBR, 0);
            m.move[DFR] = MoveResult(DLF, 0);
            m.move[DLF] = MoveResult(DBL, 0);
            m.move[DBL] = MoveResult(DRB, 0);
            m.move[DRB] = MoveResult(DFR, 0);

            m.move[UR] = MoveResult(UR, 0);
            m.move[UF] = MoveResult(UF, 0);
            m.move[UL] = MoveResult(UL, 0);
            m.move[UB] = MoveResult(UB, 0);
            m.move[DR] = MoveResult(DF, 0);
            m.move[DF] = MoveResult(DL, 0);
            m.move[DL] = MoveResult(DB, 0);
            m.move[DB] = MoveResult(DR, 0);
            m.move[FR] = MoveResult(FR, 0);
            m.move[FL] = MoveResult(FL, 0);
            m.move[BL] = MoveResult(BL, 0);
            m.move[BR] = MoveResult(BR, 0);
        }

        static inline void setAsF(CubeStatus &m) {
            m.move[URF] = MoveResult(UFL, 1);
            m.move[UFL] = MoveResult(DLF, 2);
            m.move[ULB] = MoveResult(ULB, 0);
            m.move[UBR] = MoveResult(UBR, 0);
            m.move[DFR] = MoveResult(URF, 2);
            m.move[DLF] = MoveResult(DFR, 1);
            m.move[DBL] = MoveResult(DBL, 0);
            m.move[DRB] = MoveResult(DRB, 0);

            m.move[UR] = MoveResult(UR, 0);
            m.move[UF] = MoveResult(FL, 1);
            m.move[UL] = MoveResult(UL, 0);
            m.move[UB] = MoveResult(UB, 0);
            m.move[DR] = MoveResult(DR, 0);
            m.move[DF] = MoveResult(FR, 1);
            m.move[DL] = MoveResult(DL, 0);
            m.move[DB] = MoveResult(DB, 0);
            m.move[FR] = MoveResult(UF, 1);
            m.move[FL] = MoveResult(DF, 1);
            m.move[BL] = MoveResult(BL, 0);
            m.move[BR] = MoveResult(BR, 0);
        }

        static inline void setAsB(CubeStatus &m) {

            m.move[URF] = MoveResult(URF, 0);
            m.move[UFL] = MoveResult(UFL, 0);
            m.move[ULB] = MoveResult(UBR, 1);
            m.move[UBR] = MoveResult(DRB, 2);
            m.move[DFR] = MoveResult(DFR, 0);
            m.move[DLF] = MoveResult(DLF, 0);
            m.move[DBL] = MoveResult(ULB, 2);
            m.move[DRB] = MoveResult(DBL, 1);

            m.move[UR] = MoveResult(UR, 0);
            m.move[UF] = MoveResult(UF, 0);
            m.move[UL] = MoveResult(UL, 0);
            m.move[UB] = MoveResult(BR, 1);
            m.move[DR] = MoveResult(DR, 0);
            m.move[DF] = MoveResult(DF, 0);
            m.move[DL] = MoveResult(DL, 0);
            m.move[DB] = MoveResult(BL, 1);
            m.move[FR] = MoveResult(FR, 0);
            m.move[FL] = MoveResult(FL, 0);
            m.move[BL] = MoveResult(UB, 1);
            m.move[BR] = MoveResult(DB, 1);
        }

        static inline void setAsS_URF3(CubeStatus &m) {
            // TODO
        }

        static inline void setAsS_F2(CubeStatus &m) {
            // TODO
        }

        static inline void setAsS_U4(CubeStatus &m) {
            // TODO
        }

        static inline void setAsS_LR2(CubeStatus &m) {
            // TODO
        }
    };
}

#endif //CUBE_SOLVER_CUBE_DEF_H
