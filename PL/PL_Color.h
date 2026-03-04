#pragma once
#include <WinDef.h>

struct ColorRGBA {
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE a;
	ColorRGBA() : r(0), g(0), b(0), a(0) {} // constructor mặc định
	ColorRGBA(BYTE red, BYTE green, BYTE blue, BYTE alpha) : r(red), g(green), b(blue), a(alpha) {}
};


static const ColorRGBA headerTableColor(109, 25, 192, 255); //eColorAdvance
static const ColorRGBA whiteColor(255, 255, 255, 255); //eGray 100
static const ColorRGBA gray100Color(211, 211, 211, 100); //eGray 100
static const ColorRGBA gray150Color(211, 211, 211, 150); //eGray 100
static const ColorRGBA normalColor(50, 50, 50, 255); //eGray 255
static const ColorRGBA hoverColor(1, 223, 119, 190); //eAncient 190
static const ColorRGBA inputFocusColor(69, 39, 18, 255); //eBrown
static const ColorRGBA textShopColor(0xFFu, 0xDCu, 0x96u, 0xFFu); //eTextShop
static const ColorRGBA excellentS15Color(67, 204, 65, 255);
static const ColorRGBA orangeColor(255, 105, 25, 255);
static const ColorRGBA redColor(225, 0, 0, 255);
static const ColorRGBA noColor(0, 0, 0, 0);
static const ColorRGBA blackColor(0, 0, 0, 255);
static const ColorRGBA red2Color(225, 95, 45, 255);
static const ColorRGBA eExcellentS15(67, 204, 65, 255);
static const ColorRGBA eGoldColor(255, 189, 25, 255);
static const ColorRGBA eYellowColor(255, 255, 0, 255);

//eTextShop			= Color4f(0xFFu, 0xDCu, 0x96u, 0xFFu),
//eExcellentS15		= Color4f(67, 204, 65, 255),
//eBlack			= Color4f(0, 0, 0, 255),
//eShinyGreen		= Color4f(172, 255, 56, 255),
//eShinyGreen100	= Color4f(172, 255, 56, 100),
//eGold				= Color4f(255, 189, 25, 255),
//eGold150			= Color4f(255, 189, 25, 150),
//eGold180			= Color4f(255, 189, 25, 180),
//eWhite			= Color4f(255, 255, 255, 255),
//eWhite150			= Color4f(255, 255, 255, 150),
//eWhite180			= Color4f(255, 255, 255, 180),
//eOrange			= Color4f(255, 105, 25, 255),
//eGray100			= Color4f(211, 211, 211, 100),
//eGray150			= Color4f(50, 50, 50, 150),
//eGray190			= Color4f(50, 50, 50, 190),
//eBlowPink			= Color4f(220, 20, 60, 255),
//eRed				= Color4f(225, 0, 0, 255),
//eRed2				= Color4f(225, 95, 45, 255),
//eExcellent		= Color4f(0, 225, 139, 255),
//eAncient			= Color4f(1, 223, 119, 255),
//eSocket			= Color4f(153, 102, 204, 255),
//eBlue				= Color4f(36, 242, 252, 255),
//eBlue250			= Color4f(36, 242, 252, 250),
//eYellow			= Color4f(255, 255, 0, 255),
//eYellow170		= Color4f(255, 255, 0, 170),
//eBrown			= Color4f(69, 39, 18, 255),
//eColorAdvance		= Color4f(109, 25, 192, 255),