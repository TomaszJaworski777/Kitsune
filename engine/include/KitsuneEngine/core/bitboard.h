#pragma once

#include <bit>
#include <functional>

#include "square.h"

template<typename Function>
concept BitboardIteratorCallback = requires( Function &&func, Square sq )
{
    { func( sq ) } -> std::same_as<void>;
};

struct Bitboard {
    private:
        uint64_t m_Value = 0;

    public:
        static constexpr uint64_t RANK_1 = 0x00000000000000FF;
        static constexpr uint64_t RANK_2 = 0x000000000000FF00;
        static constexpr uint64_t RANK_3 = 0x0000000000FF0000;
        static constexpr uint64_t RANK_4 = 0x00000000FF000000;
        static constexpr uint64_t RANK_5 = 0x000000FF00000000;
        static constexpr uint64_t RANK_6 = 0x0000FF0000000000;
        static constexpr uint64_t RANK_7 = 0x00FF000000000000;
        static constexpr uint64_t RANK_8 = 0xFF00000000000000;

        static constexpr uint64_t FILE_A = 0x0101010101010101;
        static constexpr uint64_t FILE_B = 0x0202020202020202;
        static constexpr uint64_t FILE_C = 0x0404040404040404;
        static constexpr uint64_t FILE_D = 0x0808080808080808;
        static constexpr uint64_t FILE_E = 0x1010101010101010;
        static constexpr uint64_t FILE_F = 0x2020202020202020;
        static constexpr uint64_t FILE_G = 0x4040404040404040;
        static constexpr uint64_t FILE_H = 0x8080808080808080;

        static constexpr uint64_t FILE_AB = FILE_A | FILE_B;
        static constexpr uint64_t FILE_GH = FILE_G | FILE_H;

        static constexpr uint64_t FULL = 0xFFFFFFFFFFFFFFFF;
        static constexpr uint64_t EMPTY = 0;

        Bitboard( const uint64_t value = 0 ) {
            m_Value = value;
        }

        Bitboard( const Square value ) {
            m_Value = 1ull << value;
        }

        void SetBit( const Square square ) {
            m_Value |= Bitboard( square );
        }

        void PopBit( const Square square ) {
            m_Value &= ~Bitboard( square );
        }

        void SwapBytes() {
            m_Value = std::byteswap( m_Value );
        }

        [[nodiscard]]
        bool GetBit( const Square square ) const {
            return ( m_Value & Bitboard( square ) ) != 0;
        }

        [[nodiscard]]
        Square PopLs1bBit() {
            const Square sq = Ls1bSquare();
            m_Value &= m_Value - 1;
            return sq;
        }

        [[nodiscard]]
        uint32_t PopCount() const {
            return std::popcount( m_Value );
        }

        [[nodiscard]]
        Square Ls1bSquare() const {
            return std::countr_zero( m_Value );
        }

        template<BitboardIteratorCallback Function>
        void Map( const Function &func ) const {
            auto bitboard = Bitboard( m_Value );
            while ( bitboard ) {
                func( bitboard.PopLs1bBit() );
            }
        }

        [[nodiscard]]
        std::string ToString() const;

        operator uint64_t() const {
            return m_Value;
        }

        Bitboard &operator=( const uint64_t value ) {
            m_Value = value;
            return *this;
        }

        Bitboard operator&( const Bitboard value ) const {
            return Bitboard{ m_Value & value.m_Value };
        }

        Bitboard operator|( const Bitboard value ) const {
            return Bitboard{ m_Value | value.m_Value };
        }

        Bitboard operator&( const uint64_t value ) const {
            return Bitboard{ m_Value & value };
        }

        Bitboard operator|( const uint64_t value ) const {
            return Bitboard{ m_Value | value };
        }

        bool operator==( const Bitboard value ) const {
            return m_Value == value.m_Value;
        }
};
