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

    //**********************************************************************
    // Declarations
    //**********************************************************************

    class MoveResult {
    public:
        BlockPos pos_;
        int8_t ori_;

        MoveResult() = default;

        MoveResult(BlockPos pos, int8_t ori);

        MoveResult(int pos, int8_t ori);

        bool operator==(const MoveResult &r) const;

        bool operator!=(const MoveResult &r) const;
    };


    class CubeStatus {
    public:
        CubeStatus() = default;

        MoveResult move_[20];

        // Position is replaced-by. Orientation is in-place.
        MoveResult operator()(BlockPos pos);

        uint64_t getConorOriCoord();

        uint64_t getEdgeOriCoord();

        uint64_t getConorPermCoord();

        uint64_t getEdgePermCoord();

        uint64_t getUDSliceCoord();

        /*
         * In phase 2, we only use the subgroup <U_, D_, L_^2, R_^2, F_^2, B_^2>
         * */

        uint64_t getPhase2EdgePermCoord();

        uint64_t getUDSliceSortedCoord();


        // Composition Operators
        CubeStatus &operator*=(const CubeStatus &other);

        CubeStatus operator*(const CubeStatus &other) const;

        CubeStatus inverse() const;

        bool operator==(const CubeStatus &c) const;

        bool operator!=(const CubeStatus &c) const;
    };


    class MoveFactory {
    public:
        // Prevent instantiation
        MoveFactory(MoveFactory &) = delete;

        void operator=(MoveFactory &) = delete;

        static MoveFactory &getInstance();

        inline CubeStatus &getMoveByEnum(BasicMoveName m);

        inline CubeStatus genRandomCube(uint64_t randomStep = 50);

        inline CubeStatus getByUDSliceCoord(uint64_t coord);

        // Identity move_
        CubeStatus Id_;

        // Basic move_s
        CubeStatus L_, R_, U_, D_, F_, B_;
        //        CubeStatus Li, Ri, Ui, Di, Fi, Bi;

        CubeStatus Sym_[48];
    private:
        // Private constructors
        MoveFactory();

    protected:
        // Basic symmetric move_s
        CubeStatus S_URF3, S_F2, S_U4, S_LR2;

        static inline void setAsId(CubeStatus &m);

        static inline void setAsL(CubeStatus &m);

        static inline void setAsR(CubeStatus &m);

        static inline void setAsU(CubeStatus &m);

        static inline void setAsD(CubeStatus &m);

        static inline void setAsF(CubeStatus &m);

        static inline void setAsB(CubeStatus &m);

        static inline void setAsS_URF3(CubeStatus &m);

        static inline void setAsS_F2(CubeStatus &m);

        static inline void setAsS_U4(CubeStatus &m);

        static inline void setAsS_LR2(CubeStatus &m);
    };

    //**********************************************************************
    // Implementations
    //**********************************************************************

    MoveResult::MoveResult(BlockPos pos, int8_t ori) : pos_(pos), ori_(ori) {}

    MoveResult::MoveResult(int pos, int8_t ori) : pos_(static_cast<BlockPos>(pos)), ori_(ori) {}

    bool MoveResult::operator==(const MoveResult &r) const {
        return this->pos_ == r.pos_ && this->ori_ == r.ori_;
    }

    bool MoveResult::operator!=(const MoveResult &r) const {
        return !this->operator==(r);
    }


    MoveResult CubeStatus::operator()(BlockPos pos) {
        return move_[pos];
    }

    uint64_t CubeStatus::getConorOriCoord() {
        int co = 0;
        // Summation of conor orientations is divided by 3.
        for (int i = BlockPos::URF; i < BlockPos::DRB; ++i) {
            co = 3 * co + move_[i].ori_;
        }
        return co;
    }

    uint64_t CubeStatus::getEdgeOriCoord() {
        int co = 0;
        // Summation of edge orientations is divided by 2.
        for (int i = BlockPos::UR; i < BlockPos::BR; ++i) {
            co = 2 * co + move_[i].ori_;
        }
        return co;
    }

    uint64_t CubeStatus::getConorPermCoord() {
        int co = 0;
        for (int i = BlockPos::DRB; i > BlockPos::URF; --i) {
            int s = 0;
            for (int j = i - 1; j >= BlockPos::URF; --j) {
                if (move_[j].pos_ > move_[i].pos_) {
                    s += 1;
                }
            }
            // (i + 1) is the order of this conor.
            co = (co + s) * (i + 1);
        }
        return co;
    }

    uint64_t CubeStatus::getEdgePermCoord() {
        int co = 0;

        for (int i = BlockPos::BR; i > BlockPos::UR; --i) {
            int s = 0;
            for (int j = i - 1; j >= BlockPos::UR; --j) {
                if (move_[j].pos_ > move_[i].pos_) {
                    s += 1;
                }
            }
            // (i + 1 - BlockPos::UR) is the order of this edge.
            co = (co + s) * (i + 1 - BlockPos::UR);
        }
        return co;
    }

    uint64_t CubeStatus::getUDSliceCoord() {
        bool occupied[12];
        std::fill(occupied, occupied + 12, false);
        for (int i = BlockPos::UR; i <= BlockPos::BR; ++i) {
            if (move_[i].pos_ >= BlockPos::FR) {
                occupied[i - BlockPos::UR] = true;
            }
        }
        uint64_t s = 0;
        int k = 3, n = 11;
        ConstantFactory &constantFactory = ConstantFactory::getInstance();
        while (k >= 0) {
            if (occupied[n]) {
                k -= 1;
                n -= 1;
                continue;
            }
            s += static_cast<uint64_t>(constantFactory.getBiCoef(n, k));
            n -= 1;
        }
        return s;
    }

    uint64_t CubeStatus::getPhase2EdgePermCoord() {
        int co = 0;
        for (int i = BlockPos::DB; i > BlockPos::UR; --i) {
            int s = 0;
            for (int j = i - 1; j >= BlockPos::UR; --j) {
                if (move_[j].pos_ > move_[i].pos_) {
                    s += 1;
                }
            }
            // (i + 1 - BlockPos::UR) is the order of this edge.
            co = (s + co) * (i + 1 - BlockPos::UR);
        }
        return co;
    }

    uint64_t CubeStatus::getUDSliceSortedCoord() {
        BlockPos arr[4];
        int j = 0;
        for (int i = BlockPos::UR; i <= BlockPos::BR; ++i) {
            BlockPos e = move_[i].pos_;
            if (e == BlockPos::FR || e == BlockPos::FL || e == BlockPos::BL || e == BlockPos::BR) {
                arr[j] = e;
                j += 1;
            }
        }

        uint64_t co = 0;
        for (j = 3; j >= 1; --j) {
            int s = 0;
            for (int k = j - 1; k >= 0; --k) {
                if (arr[k] > arr[j]) {
                    s += 1;
                }
            }
            co = (co + s) * j;
        }
        co = co + 24ULL * getUDSliceCoord();
        return co;
    }

    CubeStatus &CubeStatus::operator*=(const CubeStatus &other) {
#ifdef DEBUG
        //            std::cout << "operator*=" << std::endl;
#endif
        for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
            BlockPos thisPos = this->move_[i].pos_;
            int8_t thisOri = this->move_[i].ori_;
            this->move_[i].pos_ = other.move_[thisPos].pos_;
            this->move_[i].ori_ = thisOri + other.move_[thisPos].ori_;
            if (i <= BlockPos::DRB && this->move_[i].ori_ >= 3) {
                this->move_[i].ori_ -= 3;
            } else if (i >= BlockPos::UR && this->move_[i].ori_ >= 2) {
                this->move_[i].ori_ -= 2;
            }
        }
        return *this;
    }

    CubeStatus CubeStatus::operator*(const CubeStatus &other) const {
        auto ret = CubeStatus(*this);
        ret *= other;
        return ret;
    }

    CubeStatus CubeStatus::inverse() const {
        CubeStatus ret;
        for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
            ret.move_[this->move_[i].pos_].pos_ = static_cast<BlockPos>(i);
            ret.move_[this->move_[i].pos_].ori_ = static_cast<int8_t>(3) - this->move_[i].ori_;
            if (ret.move_[i].ori_ >= 3) {
                ret.move_[i].ori_ -= 3;
            }
        }
        return ret;
    }

    bool CubeStatus::operator==(const CubeStatus &c) const {
        for (int i = 0; i <= BlockPos::BR; ++i) {
            if (this->move_[i] != c.move_[i]) {
                return false;
            }
        }
        return true;
    }

    bool CubeStatus::operator!=(const CubeStatus &c) const {
        return !this->operator==(c);
    }

    MoveFactory &MoveFactory::getInstance() {
        static MoveFactory INSTANCE;
        return INSTANCE;
    }

    CubeStatus &MoveFactory::getMoveByEnum(BasicMoveName m) {
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
                throw std::runtime_error(std::string(__func__) + "Unexpected move_ name");
        }
    }

    CubeStatus MoveFactory::genRandomCube(uint64_t randomStep) {
        auto &randomFactory = RandomFactory::getInstance();
        CubeStatus cur = Id_;
        std::uniform_int_distribution<> dist;
        while (randomStep-- > 0) {
#ifdef DEBUG
//            std::cout << "randomStep: " << randomStep << std::endl;
#endif
            auto move_Enum = static_cast<BasicMoveName>(dist(randomFactory.generator_) % 6);
            CubeStatus move_ = getMoveByEnum(move_Enum);
            cur *= move_;
        }
        return cur;
    }

    MoveFactory::MoveFactory() {
        // Init id move_
        setAsId(Id_);

        // Init basic move_s
        setAsU(U_);
        setAsD(D_);
        setAsL(L_);
        setAsR(R_);
        setAsF(F_);
        setAsB(B_);

        // Init inverse of basic move_s
        //            Li = L_.inverse();
        //            Ri = R_.inverse();
        //            Ui = U_.inverse();
        //            Di = D_.inverse();
        //            Fi = F_.inverse();
        //            Bi = B_.inverse();

        // Init basic symmetric move_s
        setAsS_URF3(S_URF3);
        setAsS_F2(S_F2);
        setAsS_U4(S_U4);
        setAsS_LR2(S_LR2);

        // Init all symmetric move_s
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

    void MoveFactory::setAsId(CubeStatus &m) {
        for (int i = BlockPos::URF; i <= BlockPos::BR; ++i) {
            m.move_[i].pos_ = static_cast<BlockPos>(i);
            m.move_[i].ori_ = 0;
        }
    }

    void MoveFactory::setAsL(CubeStatus &m) {

        m.move_[URF] = MoveResult(URF, 0);
        m.move_[UFL] = MoveResult(ULB, 1);
        m.move_[ULB] = MoveResult(DBL, 2);
        m.move_[UBR] = MoveResult(UBR, 0);
        m.move_[DFR] = MoveResult(DFR, 0);
        m.move_[DLF] = MoveResult(UFL, 2);
        m.move_[DBL] = MoveResult(DLF, 1);
        m.move_[DRB] = MoveResult(DRB, 0);

        m.move_[UR] = MoveResult(UR, 0);
        m.move_[UF] = MoveResult(UF, 0);
        m.move_[UL] = MoveResult(BL, 0);
        m.move_[UB] = MoveResult(UB, 0);
        m.move_[DR] = MoveResult(DR, 0);
        m.move_[DF] = MoveResult(DF, 0);
        m.move_[DL] = MoveResult(FL, 0);
        m.move_[DB] = MoveResult(DB, 0);
        m.move_[FR] = MoveResult(FR, 0);
        m.move_[FL] = MoveResult(UL, 0);
        m.move_[BL] = MoveResult(DL, 0);
        m.move_[BR] = MoveResult(BR, 0);
    }

    void MoveFactory::setAsR(CubeStatus &m) {
        m.move_[URF] = MoveResult(DFR, 2);
        m.move_[UFL] = MoveResult(UFL, 0);
        m.move_[ULB] = MoveResult(ULB, 0);
        m.move_[UBR] = MoveResult(URF, 1);
        m.move_[DFR] = MoveResult(DRB, 1);
        m.move_[DLF] = MoveResult(DLF, 0);
        m.move_[DBL] = MoveResult(DBL, 0);
        m.move_[DRB] = MoveResult(UBR, 2);

        m.move_[UR] = MoveResult(FR, 0);
        m.move_[UF] = MoveResult(UF, 0);
        m.move_[UL] = MoveResult(UL, 0);
        m.move_[UB] = MoveResult(UB, 0);
        m.move_[DR] = MoveResult(BR, 0);
        m.move_[DF] = MoveResult(DF, 0);
        m.move_[DL] = MoveResult(DL, 0);
        m.move_[DB] = MoveResult(DB, 0);
        m.move_[FR] = MoveResult(DR, 0);
        m.move_[FL] = MoveResult(FL, 0);
        m.move_[BL] = MoveResult(BL, 0);
        m.move_[BR] = MoveResult(UR, 0);

    }

    void MoveFactory::setAsU(CubeStatus &m) {
        m.move_[URF] = MoveResult(UBR, 0);
        m.move_[UFL] = MoveResult(URF, 0);
        m.move_[ULB] = MoveResult(UFL, 0);
        m.move_[UBR] = MoveResult(ULB, 0);
        m.move_[DFR] = MoveResult(DFR, 0);
        m.move_[DLF] = MoveResult(DLF, 0);
        m.move_[DBL] = MoveResult(DBL, 0);
        m.move_[DRB] = MoveResult(DRB, 0);


        m.move_[UR] = MoveResult(UB, 0);
        m.move_[UF] = MoveResult(UR, 0);
        m.move_[UL] = MoveResult(UF, 0);
        m.move_[UB] = MoveResult(UL, 0);
        m.move_[DR] = MoveResult(DR, 0);
        m.move_[DF] = MoveResult(DF, 0);
        m.move_[DL] = MoveResult(DL, 0);
        m.move_[DB] = MoveResult(DB, 0);
        m.move_[FR] = MoveResult(FR, 0);
        m.move_[FL] = MoveResult(FL, 0);
        m.move_[BL] = MoveResult(BL, 0);
        m.move_[BR] = MoveResult(BR, 0);
    }

    void MoveFactory::setAsD(CubeStatus &m) {

        m.move_[URF] = MoveResult(URF, 0);
        m.move_[UFL] = MoveResult(UFL, 0);
        m.move_[ULB] = MoveResult(ULB, 0);
        m.move_[UBR] = MoveResult(UBR, 0);
        m.move_[DFR] = MoveResult(DLF, 0);
        m.move_[DLF] = MoveResult(DBL, 0);
        m.move_[DBL] = MoveResult(DRB, 0);
        m.move_[DRB] = MoveResult(DFR, 0);

        m.move_[UR] = MoveResult(UR, 0);
        m.move_[UF] = MoveResult(UF, 0);
        m.move_[UL] = MoveResult(UL, 0);
        m.move_[UB] = MoveResult(UB, 0);
        m.move_[DR] = MoveResult(DF, 0);
        m.move_[DF] = MoveResult(DL, 0);
        m.move_[DL] = MoveResult(DB, 0);
        m.move_[DB] = MoveResult(DR, 0);
        m.move_[FR] = MoveResult(FR, 0);
        m.move_[FL] = MoveResult(FL, 0);
        m.move_[BL] = MoveResult(BL, 0);
        m.move_[BR] = MoveResult(BR, 0);
    }

    void MoveFactory::setAsF(CubeStatus &m) {
        m.move_[URF] = MoveResult(UFL, 1);
        m.move_[UFL] = MoveResult(DLF, 2);
        m.move_[ULB] = MoveResult(ULB, 0);
        m.move_[UBR] = MoveResult(UBR, 0);
        m.move_[DFR] = MoveResult(URF, 2);
        m.move_[DLF] = MoveResult(DFR, 1);
        m.move_[DBL] = MoveResult(DBL, 0);
        m.move_[DRB] = MoveResult(DRB, 0);

        m.move_[UR] = MoveResult(UR, 0);
        m.move_[UF] = MoveResult(FL, 1);
        m.move_[UL] = MoveResult(UL, 0);
        m.move_[UB] = MoveResult(UB, 0);
        m.move_[DR] = MoveResult(DR, 0);
        m.move_[DF] = MoveResult(FR, 1);
        m.move_[DL] = MoveResult(DL, 0);
        m.move_[DB] = MoveResult(DB, 0);
        m.move_[FR] = MoveResult(UF, 1);
        m.move_[FL] = MoveResult(DF, 1);
        m.move_[BL] = MoveResult(BL, 0);
        m.move_[BR] = MoveResult(BR, 0);
    }

    void MoveFactory::setAsB(CubeStatus &m) {

        m.move_[URF] = MoveResult(URF, 0);
        m.move_[UFL] = MoveResult(UFL, 0);
        m.move_[ULB] = MoveResult(UBR, 1);
        m.move_[UBR] = MoveResult(DRB, 2);
        m.move_[DFR] = MoveResult(DFR, 0);
        m.move_[DLF] = MoveResult(DLF, 0);
        m.move_[DBL] = MoveResult(ULB, 2);
        m.move_[DRB] = MoveResult(DBL, 1);

        m.move_[UR] = MoveResult(UR, 0);
        m.move_[UF] = MoveResult(UF, 0);
        m.move_[UL] = MoveResult(UL, 0);
        m.move_[UB] = MoveResult(BR, 1);
        m.move_[DR] = MoveResult(DR, 0);
        m.move_[DF] = MoveResult(DF, 0);
        m.move_[DL] = MoveResult(DL, 0);
        m.move_[DB] = MoveResult(BL, 1);
        m.move_[FR] = MoveResult(FR, 0);
        m.move_[FL] = MoveResult(FL, 0);
        m.move_[BL] = MoveResult(UB, 1);
        m.move_[BR] = MoveResult(DB, 1);
    }

    void MoveFactory::setAsS_URF3(CubeStatus &m) {
        // TODO
    }

    void MoveFactory::setAsS_F2(CubeStatus &m) {
        // TODO
    }

    void MoveFactory::setAsS_U4(CubeStatus &m) {
        // TODO
    }

    void MoveFactory::setAsS_LR2(CubeStatus &m) {
        // TODO
    }

    CubeStatus MoveFactory::getByUDSliceCoord(uint64_t coord) {
        auto res = Id_;
        auto &CONSTANTS = ConstantFactory::getInstance();
        bool occ[12];

        std::fill(occ, occ + 12, false);
        int pos = 11;
        uint64_t cur = coord;
        for (int j = 3; j >= 0; --j) {
            while (cur >= CONSTANTS.getBiCoef(pos, j)) {
                cur -= CONSTANTS.getBiCoef(pos, j);
                pos--;
            }
            occ[pos--] = true;
        }

        auto pos1 = BlockPos::BR, pos2 = BlockPos::DB;
        for (int i = 0; i < 12; ++i) {
            if (occ[i]) {
                res.move_[i + BlockPos::UR] = MoveResult(pos1, 0);
                pos1 = static_cast<BlockPos>(pos1 - 1);
            } else {
                res.move_[i + BlockPos::UR] = MoveResult(pos2, 0);
                pos2 = static_cast<BlockPos>(pos2 - 1);
            }
        }
        return res;
    }
}


#endif //CUBE_SOLVER_CUBE_DEF_H
