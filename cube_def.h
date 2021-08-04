#ifndef CUBE_SOLVER_CUBE_DEF_H
#define CUBE_SOLVER_CUBE_DEF_H

#include <array>
#include <cstdint>
#include <algorithm>

#include "utility.h"

namespace Cube {

    class MoveResult {
    public:
        BlockPos pos_;
        int8_t ori_;
    };

    class IMove {
    public:
        IMove() = default;

        IMove(BlockPos pos, int8_t ori_) : pos_(pos), ori_(ori) {}

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
            ConstantFactory &CONSTANT = ConstantFactory.getInstance();
            while (k >= 0) {
                if (occupied[n]) {
                    k -= 1;
                }
                s += static_cast<int>(CONSTANT.getBinomialCoefficient(n, k));
                n -= 1;
            }
            return s;
        }

        /*
         * In phase 2, we only use the subgroup <U, D, L^2, R^2, F^2, B^2>
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
        }

        int getUDSliceSortedCoord() {
            BlockPos arr[4];
            for (int i = BlockPos::UR; i <= BlockPos::BR; ++i) {
                BlockPos e = move[i].pos_;
                if (e == BlockPos::FR || e == BlockPos::FL || e == BlockPos::BL || e == BlockPos::BR) {
                    arr[j] = e;
                    j += 1;
                }
            }

            int co = 0;
            for (int j = 3; j >= 1; --j) {
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
        friend IMove &operator*=(const IMove &other) {
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                BlockPos otherPos = other.move[i].pos_;
                int otherOri = other.move[i].ori_;
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

        friend IMove operator*(const IMove &other) const {
            IMove ret = IMove(*this);
            ret *= other;
            return ret;
        }

        IMove inverse() const {
            IMove ret;
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                ret.move[this->move[i].pos_].pos_ = i;
                ret.move[this->move[i].pos_].ori_ = static_cast<int8_t>(3) - this->move[i].ori_;
                if (ret.move[i].ori_ >= 3) {
                    ret.move[i].ori_ -= 3;
                }
            }
            return ret;
        }
    };


    class MoveFactory {
    private:
        // Private constructors
        MoveFactory() {
            // Init id move
            setAsId(Id);

            // Init basic moves
            setAsU(U);
            setAsD(D);
            setAsL(L);
            setAsR(R);
            setAsF(F);
            setAsB(B);

            // Init inverse of basic moves
//            Li = L.inverse();
//            Ri = R.inverse();
//            Ui = U.inverse();
//            Di = D.inverse();
//            Fi = F.inverse();
//            Bi = B.inverse();

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
                            IMove S = Id;
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
                            Sym[idx] = S;
                        }
                    }
                }
            }
        }

    protected:
        // Basic symmetric moves
        IMove S_URF3, S_F2, S_U4, S_LR2;

        static void setAsId(IMove &m) {
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                m.move[i].pos_ = i;
                m.move[i].ori_ = 0;
            }
        }

        void setAsL(IMove &m) {

            m.move[URF] = IMove(URF, 0);
            m.move[UFL] = IMove(ULB, 1);
            m.move[ULB] = IMove(DBL, 2);
            m.move[UBR] = IMove(UBR, 0);
            m.move[DFR] = IMove(DFR, 0);
            m.move[DLF] = IMove(UFL, 2);
            m.move[DBL] = IMove(DLF, 1);
            m.move[DRB] = IMove(DRB, 0);

            m.move[UR] = IMove(UR, 0);
            m.move[UF] = IMove(UF, 0);
            m.move[UL] = IMove(BL, 0);
            m.move[UB] = IMove(UB, 0);
            m.move[DR] = IMove(DR, 0);
            m.move[DF] = IMove(DF, 0);
            m.move[DL] = IMove(FL, 0);
            m.move[DB] = IMove(DB, 0);
            m.move[FR] = IMove(FR, 0);
            m.move[FL] = IMove(UL, 0);
            m.move[BL] = IMove(DL, 0);
            m.move[BR] = IMove(BR, 0)
        }

        void setAsR(IMove &m) {
            m.move[URF] = IMove(DFR, 2);
            m.move[UFL] = IMove(UFL, 0);
            m.move[ULB] = IMove(ULB, 0);
            m.move[UBR] = IMove(URF, 1);
            m.move[DFR] = IMove(DRB, 1);
            m.move[DLF] = IMove(DLF, 0);
            m.move[DBL] = IMove(DBL, 0);
            m.move[DRB] = IMove(UBR, 2);

            m.move[UR] = IMove(FR, 0);
            m.move[UF] = IMove(UF, 0);
            m.move[UL] = IMove(UL, 0);
            m.move[UB] = IMove(UB, 0);
            m.move[DR] = IMove(BR, 0);
            m.move[DF] = IMove(DF, 0);
            m.move[DL] = IMove(DL, 0);
            m.move[DB] = IMove(DB, 0);
            m.move[FR] = IMove(DR, 0);
            m.move[FL] = IMove(FL, 0);
            m.move[BL] = IMove(BL, 0);
            m.move[BR] = IMove(UR, 0);

        }

        void setAsU(IMove &m) {
            m.move[URF] = IMove(UBR, 0);
            m.move[UFL] = IMove(URF, 0);
            m.move[ULB] = IMove(UFL, 0);
            m.move[UBR] = IMove(ULB, 0);
            m.move[DFR] = IMove(DFR, 0);
            m.move[DLF] = IMove(DLF, 0);
            m.move[DBL] = IMove(DBL, 0);
            m.move[DRB] = IMove(DRB, 0);


            m.move[UR] = IMove(UB, 0);
            m.move[UF] = IMove(UR, 0);
            m.move[UL] = IMove(UF, 0);
            m.move[UB] = IMove(UL, 0);
            m.move[DR] = IMove(DR, 0);
            m.move[DF] = IMove(DF, 0);
            m.move[DL] = IMove(DL, 0);
            m.move[DB] = IMove(DB, 0);
            m.move[FR] = IMove(FR, 0);
            m.move[FL] = IMove(FL, 0);
            m.move[BL] = IMove(BL, 0);
            m.move[BR] = IMove(BR, 0);
        }

        void setAsD(IMove &m) {

            m.move[URF] = IMove(URF, 0);
            m.move[UFL] = IMove(UFL, 0);
            m.move[ULB] = IMove(ULB, 0);
            m.move[UBR] = IMove(UBR, 0);
            m.move[DFR] = IMove(DLF, 0);
            m.move[DLF] = IMove(DBL, 0);
            m.move[DBL] = IMove(DRB, 0);
            m.move[DRB] = IMove(DFR, 0);

            m.move[UR] = IMove(UR, 0);
            m.move[UF] = IMove(UF, 0);
            m.move[UL] = IMove(UL, 0);
            m.move[UB] = IMove(UB, 0);
            m.move[DR] = IMove(DF, 0);
            m.move[DF] = IMove(DL, 0);
            m.move[DL] = IMove(DB, 0);
            m.move[DB] = IMove(DR, 0);
            m.move[FR] = IMove(FR, 0);
            m.move[FL] = IMove(FL, 0);
            m.move[BL] = IMove(BL, 0);
            m.move[BR] = IMove(BR, 0);
        }

        void setAsF(IMove &m) {
            m.move[URF] = IMove(UFL, 1);
            m.move[UFL] = IMove(DLF, 2);
            m.move[ULB] = IMove(ULB, 0);
            m.move[UBR] = IMove(UBR, 0);
            m.move[DFR] = IMove(URF, 2);
            m.move[DLF] = IMove(DFR, 1);
            m.move[DBL] = IMove(DBL, 0);
            m.move[DRB] = IMove(DRB, 0);

            m.move[UR] = IMove(UR, 0);
            m.move[UF] = IMove(FL, 1);
            m.move[UL] = IMove(UL, 0);
            m.move[UB] = IMove(UB, 0);
            m.move[DR] = IMove(DR, 0);
            m.move[DF] = IMove(FR, 1);
            m.move[DL] = IMove(DL, 0);
            m.move[DB] = IMove(DB, 0);
            m.move[FR] = IMove(UF, 1);
            m.move[FL] = IMove(DF, 1);
            m.move[BL] = IMove(BL, 0);
            m.move[BR] = IMove(BR, 0);
        }

        void setAsB(IMove &m) {

            m.move[URF] = IMove(URF, 0);
            m.move[UFL] = IMove(UFL, 0);
            m.move[ULB] = IMove(UBR, 1);
            m.move[UBR] = IMove(DRB, 2);
            m.move[DFR] = IMove(DFR, 0);
            m.move[DLF] = IMove(DLF, 0);
            m.move[DBL] = IMove(ULB, 2);
            m.move[DRB] = IMove(DBL, 1);

            m.move[UR] = IMove(UR, 0);
            m.move[UF] = IMove(UF, 0);
            m.move[UL] = IMove(UL, 0);
            m.move[UB] = IMove(BR, 1);
            m.move[DR] = IMove(DR, 0);
            m.move[DF] = IMove(DF, 0);
            m.move[DL] = IMove(DL, 0);
            m.move[DB] = IMove(BL, 1);
            m.move[FR] = IMove(FR, 0);
            m.move[FL] = IMove(FL, 0);
            m.move[BL] = IMove(UB, 1);
            m.move[BR] = IMove(DB, 1);
        }

        void setAsS_URF3(IMove &m) {
            // TODO
        }

        void setAsS_F2(IMove &m) {
            // TODO
        }

        void setAsS_U4(IMove &m) {
            // TODO
        }

        void setAsS_LR2(IMove &m) {
            // TODO
        }

    public:
        // Prevent instantiation
        MoveFactory(MoveFactory &) = delete;

        MoveFactory(MoveFactory &&) = delete;

        static MoveFactory &getInstance() {
            static MoveFactory INSTANCE = MoveFactory();
            return INSTANCE;
        }

        // Identity move
        IMove Id;

        // Basic moves
        IMove L, R, U, D, F, B;
//        IMove Li, Ri, Ui, Di, Fi, Bi;

        IMove Sym[48];
    };
}

#endif //CUBE_SOLVER_CUBE_DEF_H
