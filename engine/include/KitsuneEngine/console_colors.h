#pragma once

#include <string>

struct Color {
	private:
		uint8_t m_Red = 0;
		uint8_t m_Green = 0;
		uint8_t m_Blue = 0;
	public:
		constexpr Color( const uint8_t red, const uint8_t green, const uint8_t blue ) {
			m_Red = red;
			m_Green = green;
			m_Blue = blue;
		}

		[[nodiscard]]
		uint8_t Red() const {
			return m_Red;
		}

		[[nodiscard]]
		uint8_t Green() const {
			return m_Green;
		}

		[[nodiscard]]
		uint8_t Blue() const {
			return m_Blue;
		}
};

std::string ColorText( const std::string &str, const Color& color );

constexpr auto RED = Color( 255, 75, 75 );
constexpr auto GREEN = Color( 75, 255, 75 );
constexpr auto BLUE = Color( 100, 100, 255 );
constexpr auto DARK_BLUE = Color( 75, 75, 200 );
constexpr auto YELLOW = Color( 255, 255, 100 );

