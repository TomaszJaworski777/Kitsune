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

        constexpr Bitboard( const uint64_t value = 0 ) {
            m_Value = value;
        }

        constexpr Bitboard( const Square value ) {
            m_Value = 1ull << value;
        }

        constexpr void SetBit( const Square square ) {
            m_Value |= Bitboard( square );
        }

        constexpr void PopBit( const Square square ) {
            m_Value &= ~Bitboard( square );
        }

        constexpr void SwapBytes() {
            m_Value = std::byteswap( m_Value );
        }

        [[nodiscard]]
        constexpr bool GetBit( const Square square ) const {
            return ( m_Value & Bitboard( square ) ) != 0;
        }

        [[nodiscard]]
        constexpr Square PopLs1bBit() {
            const Square sq = Ls1bSquare();
            m_Value &= m_Value - 1;
            return sq;
        }

        [[nodiscard]]
        constexpr uint32_t PopCount() const {
            return std::popcount( m_Value );
        }

        [[nodiscard]]
        constexpr bool OnlyOneBit() const {
            return m_Value != 0 && !( m_Value & ( m_Value - 1 ) );
        }

        [[nodiscard]]
        constexpr bool MultipleOneBits() const {
            return m_Value != 0 && ( m_Value & ( m_Value - 1 ) );
        }

        [[nodiscard]]
        constexpr Square Ls1bSquare() const {
            return std::countr_zero( m_Value );
        }

        template<BitboardIteratorCallback Function>
        constexpr void Map( const Function &func ) const {
            auto bitboard = Bitboard( m_Value );
            while ( bitboard ) {
                func( bitboard.PopLs1bBit() );
            }
        }

        [[nodiscard]]
        std::string ToString() const;

        constexpr operator uint64_t() const {
            return m_Value;
        }

        constexpr Bitboard &operator=( const uint64_t value ) {
            m_Value = value;
            return *this;
        }

        constexpr Bitboard operator&( const Bitboard value ) const {
            return Bitboard{ m_Value & value.m_Value };
        }

        constexpr Bitboard operator|( const Bitboard value ) const {
            return Bitboard{ m_Value | value.m_Value };
        }

        constexpr Bitboard operator&( const uint64_t value ) const {
            return Bitboard{ m_Value & value };
        }

        constexpr Bitboard operator|( const uint64_t value ) const {
            return Bitboard{ m_Value | value };
        }

        constexpr Bitboard &operator&=( const Bitboard value ) {
            m_Value &= value.m_Value;
            return *this;
        }

        constexpr Bitboard &operator|=( const Bitboard value ) {
            m_Value |= value.m_Value;
            return *this;
        }

        constexpr Bitboard &operator&=( const uint64_t value ) {
            m_Value &= value;
            return *this;
        }

        constexpr Bitboard &operator|=( const uint64_t value ) {
            m_Value |= value;
            return *this;
        }

        constexpr bool operator==( const Bitboard value ) const {
            return m_Value == value.m_Value;
        }
};
