//
// Created by LinHeLurking on 2021-08-03.
//

#ifndef CUBE_SOLVER_CUBE_DEF_H
#define CUBE_SOLVER_CUBE_DEF_H

#include <array>
#include <cstdint>
#include <algorithm>

namespace Cube {

    enum Color {
        Green, Yellow, Red, Blue, Orange, White
    };

    enum BlockPos {
        URF = 0, UFL, ULB, UBR, DFR, DLF, DBL, DRB, UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR
    };

    class MoveResult {
    public:
        BlockPos pos_;
        int ori_;
    };

    class IMove {
    public:
        IMove() = default;

        MoveResult move[20];

        // Position is replaced-by. Orientation is in-place.
        MoveResult operator()(BlockPos pos) {
            return move[pos];
        }

        int getConorOrientationCoordinate() {
            int co = 0;
            // Summation of conor orientations is divided by 3.
            for (int i = BlockPos::URF; i < BlockPos::DRB; ++i) {
                co = 3 * co + move[i].ori_;
            }
            return co;
        }

        int getEdgeOrientationCoordinate() {
            int co = 0;
            // Summation of edge orientations is divided by 2.
            for (int i = BlockPos::UR; i < BlockPos::BR; ++i) {
                co = 2 * co + move[i].ori_;
            }
            return co;
        }

        int getConorPermutationCoordinate() {
            int co = 0;
            for (int i = BlockPos::DRB; i > BlockPos::URF; --i) {
                int s = 0;
                for (int j = i - 1; j >= BlockPos::URF; --j) {
                    if (move[j].pos_ > move[i].pos_) {
                        s += 1;
                    }
                }
                // (i+1) is the order of this conor.
                co = (co + s) * (i + 1);
            }
            return co;
        }

        int getEdgePermutationCoordinate() {
            int co = 0;

            for (int i = BlockPos::BR; i > BlockPos::UR; --i) {
                int s = 0;
                for (int j = i - 1; j >= BlockPos::UR; --j) {
                    if (move[j].pos_ > move[i].pos_) {
                        s += 1;
                    }
                }
                // (i+1) is the order of this conor.
                co = (co + s) * (i + 1);
            }
            return co;
        }

        int getUDSliceCoordinate() {
            // TODO
        }

        friend IMove &operator*=(const IMove &other) {
            for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
                BlockPos otherPos = other.move[i].pos_;
                int otherOri = other.move[i].ori_;
                this->move[i].pos_ = this->move[otherPos].pos_;
                this->move[i].ori_ = this->move[otherPos].ori_ + otherOri;
                if (this->move[i].ori_ >= 3) {
                    this->move[i].ori_ -= 3;
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
                ret.move[this->move[i].pos_].ori_ = 3 - this->move[i].ori_;
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
            Li = L.inverse();
            Ri = R.inverse();
            Ui = U.inverse();
            Di = D.inverse();
            Fi = F.inverse();
            Bi = B.inverse();

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
            // TODO
        }

        void setAsR(IMove &m) {
            // TODO
        }

        void setAsU(IMove &m) {
            // TODO
        }

        void setAsD(IMove &m) {
            // TODO
        }

        void setAsF(IMove &m) {
            // TODO
        }

        void setAsB(IMove &m) {
            // TODO
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
            static auto INSTANCE = MoveFactory();
            return INSTANCE;
        }

        // Identity move
        IMove Id;

        // Basic moves
        IMove L, R, U, D, F, B;
        IMove Li, Ri, Ui, Di, Fi, Bi;

        IMove Sym[48];
    };
}

#endif //CUBE_SOLVER_CUBE_DEF_H
