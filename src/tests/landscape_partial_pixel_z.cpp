/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file landscape_partial_pixel_z.cpp Tests for consistency/validity of the results of GetPartialPixelZ. */

#include "../stdafx.h"

#include "../3rdparty/catch2/catch.hpp"

#include "../landscape.h"
#include "../slope_func.h"
#include <array>

/**
 * Check whether the addition of two slope's GetPartialPixelZ values results in
 * the GetPartialPixelZ values of the expected slope.
 * This iterates over all sub-pixel locations within a single tile.
 * @param slope_expected The slope that is expected.
 * @param slope_a The first slope of the addition.
 * @param slope_b The second slope of the addition.
 * @return True iff at all GetPartialPixelZ results are the same for each sub-tile position.
 */
bool CheckPartialPixelZSlopeAddition(Slope slope_expected, Slope slope_a, Slope slope_b)
{
	for (uint x = 0; x < TILE_SIZE; x++) {
		for (uint y = 0; y < TILE_SIZE; y++) {
			int z_a = GetPartialPixelZ(x, y, slope_a);
			int z_b = GetPartialPixelZ(x, y, slope_b);
			int z_result = GetPartialPixelZ(x, y, slope_expected);
			if (z_result != z_a + z_b) return false;
		}
	}
	return true;
}

TEST_CASE("PartialPixelSlopeAdditionTest - A one corner slope, plus the opposite three corner slope results in a flat but elevated slope")
{
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, SLOPE_N, SLOPE_WSE));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, SLOPE_E, SLOPE_NWS));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, SLOPE_S, SLOPE_ENW));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, SLOPE_W, SLOPE_SEN));
}

TEST_CASE("PartialPixelSlopeAdditionTest - Diagonal slopes with their opposite slope result in a flat but elevated slope")
{
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, SLOPE_NW, SLOPE_SE));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, SLOPE_SW, SLOPE_NE));
}

TEST_CASE("PartialPixelSlopeAdditionTest - Half tile slopes with their opposite half tile slope result in a flat but elevated slope")
{
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, HalftileSlope(SLOPE_N, CORNER_N), HalftileSlope(SLOPE_S, CORNER_S)));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_ELEVATED, HalftileSlope(SLOPE_E, CORNER_E), HalftileSlope(SLOPE_W, CORNER_W)));
}

TEST_CASE("PartialPixelSlopeAdditionTest - Two opposite one corner slopes result in the two corner slope with opposite corners")
{
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_NS, SLOPE_N, SLOPE_S));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_EW, SLOPE_E, SLOPE_W));
}

TEST_CASE("PartialPixelSlopeAdditionTest - A steep slope is a one corner slope on top of a three corner slope")
{
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_STEEP_N, SLOPE_N, SLOPE_ENW));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_STEEP_E, SLOPE_E, SLOPE_SEN));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_STEEP_S, SLOPE_S, SLOPE_WSE));
	CHECK(CheckPartialPixelZSlopeAddition(SLOPE_STEEP_W, SLOPE_W, SLOPE_NWS));
}

TEST_CASE("PartialPixelSlopeAdditionTest - A half tile steep slope is a one corner half tile on top of a three corner slope")
{
	CHECK(CheckPartialPixelZSlopeAddition(HalftileSlope(SLOPE_STEEP_N, CORNER_N), HalftileSlope(SLOPE_N, CORNER_N), SLOPE_ENW));
	CHECK(CheckPartialPixelZSlopeAddition(HalftileSlope(SLOPE_STEEP_E, CORNER_E), HalftileSlope(SLOPE_E, CORNER_E), SLOPE_SEN));
	CHECK(CheckPartialPixelZSlopeAddition(HalftileSlope(SLOPE_STEEP_S, CORNER_S), HalftileSlope(SLOPE_S, CORNER_S), SLOPE_WSE));
	CHECK(CheckPartialPixelZSlopeAddition(HalftileSlope(SLOPE_STEEP_W, CORNER_W), HalftileSlope(SLOPE_W, CORNER_W), SLOPE_NWS));
}

/**
 * Check whether the partial pixel Z values are the expected values. The arrays
 * are as if the map is rotated 45 degrees counterclockwise.
 * @param slope The slope that is to be checked.
 * @param expected The expect partial pixels Z values.
 * @return True iff at all GetPartialPixelZ results are the same as the expected Z-coordinates.
 */
bool CheckPartialPixelZ(Slope slope, std::array<int, TILE_SIZE * TILE_SIZE> expected)
{
	for (uint i = 0; i < expected.size(); i++) {
		int actual = GetPartialPixelZ(GB(i, 4, 4), GB(i, 0, 4), slope);
		if (actual != expected[i]) return false;
	}
	return true;
}

TEST_CASE("PartialPixelTest - One corner up slopes - SLOPE_N")
{
	CHECK(CheckPartialPixelZ(SLOPE_N, {
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0,
		7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0,
		6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0,
		6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0,
		5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0,
		5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
}

TEST_CASE("PartialPixelTest - One corner up slopes - SLOPE_E")
{
	CHECK(CheckPartialPixelZ(SLOPE_E, {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,
		0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
		0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
}

TEST_CASE("PartialPixelTest - One corner up slopes - SLOPE_S")
{
	CHECK(CheckPartialPixelZ(SLOPE_S, {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4,
		0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
		0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
		0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
		0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7}));
}

TEST_CASE("PartialPixelTest - One corner up slopes - SLOPE_W")
{
	CHECK(CheckPartialPixelZ(SLOPE_W, {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
		5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0,
		5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0,
		6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0, 0,
		6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0, 0,
		7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 0,
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0}));
}

TEST_CASE("PartialPixelTest - Two corner up, diagonal slopes - SLOPE_NE")
{
	CHECK(CheckPartialPixelZ(SLOPE_NE, {
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
}

TEST_CASE("PartialPixelTest - Two corner up, diagonal slopes - SLOPE_SE")
{
	CHECK(CheckPartialPixelZ(SLOPE_SE, {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8}));
}

TEST_CASE("PartialPixelTest - Two corner up, diagonal slopes - SLOPE_SW")
{
	CHECK(CheckPartialPixelZ(SLOPE_SW, {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}));
}

TEST_CASE("PartialPixelTest - Two corner up, diagonal slopes - SLOPE_NW")
{
	CHECK(CheckPartialPixelZ(SLOPE_NW, {
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0}));
}

TEST_CASE("PartialPixelTest - Two opposite corner up slopes - SLOPE_NS")
{
	CHECK(CheckPartialPixelZ(SLOPE_NS, {
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0,
		7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1,
		6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1,
		6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2,
		5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2,
		5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3,
		4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3,
		4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4,
		3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
		3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
		2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
		1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7}));
}

TEST_CASE("PartialPixelTest - Two opposite corner up slopes - SLOPE_EW")
{
	CHECK(CheckPartialPixelZ(SLOPE_EW, {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7,
		1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7,
		1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
		2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6,
		2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5,
		3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5,
		3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4,
		4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 4,
		4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3,
		5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2, 3,
		5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2, 2,
		6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1, 2,
		6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1, 1,
		7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0, 1,
		7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0, 0}));
}

TEST_CASE("PartialPixelTest - Three corner up slopes - SLOPE_ENW")
{
	CHECK(CheckPartialPixelZ(SLOPE_ENW, {
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4,
		8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4,
		8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3,
		8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3,
		8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2,
		8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2,
		8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1,
		8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1}));
}

TEST_CASE("PartialPixelTest - Three corner up slopes - SLOPE_SEN")
{
	CHECK(CheckPartialPixelZ(SLOPE_SEN, {
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8,
		3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8,
		3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8,
		2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8,
		2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8,
		1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8,
		1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8}));
}

TEST_CASE("PartialPixelTest - Three corner up slopes - SLOPE_WSE")
{
	CHECK(CheckPartialPixelZ(SLOPE_WSE, {
		0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8,
		1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,
		1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8,
		2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8,
		2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8,
		3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8,
		3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8,
		4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8,
		4, 5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		5, 5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		5, 6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		6, 6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		6, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		7, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}));
}

TEST_CASE("PartialPixelTest - Three corner up slopes - SLOPE_NWS")
{
	CHECK(CheckPartialPixelZ(SLOPE_NWS, {
		8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 0,
		8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1,
		8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1,
		8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2,
		8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2,
		8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3,
		8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3,
		8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 4,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 5,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}));
}

TEST_CASE("PartialPixelTest - Normal half tile slopes - SLOPE/CORNER_N")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_N, CORNER_N), {
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}));
}

TEST_CASE("PartialPixelTest - Normal half tile slopes - SLOPE/CORNER_E")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_E, CORNER_E), {
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8}));
}

TEST_CASE("PartialPixelTest - Normal half tile slopes - SLOPE/CORNER_S")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_S, CORNER_S), {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		0, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8}));
}

TEST_CASE("PartialPixelTest - Normal half tile slopes - SLOPE/CORNER_W")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_W, CORNER_W), {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0, 0,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 0}));
}

TEST_CASE("PartialPixelTest - Steep slopes - SLOPE_STEEP_N")
{
	CHECK(CheckPartialPixelZ(SLOPE_STEEP_N, {
		16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,
		15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,
		15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,
		14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,
		14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,
		13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,
		13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,
		12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,
		12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,
		11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,
		11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,
		10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,
		10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,
		 9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,
		 9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,
		 8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1}));
}

TEST_CASE("PartialPixelTest - Steep slopes - SLOPE_STEEP_E")
{
	CHECK(CheckPartialPixelZ(SLOPE_STEEP_E, {
		 8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16,
		 8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15,
		 7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15,
		 7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
		 6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14,
		 6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13,
		 5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13,
		 5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12,
		 4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12,
		 4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11,
		 3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11,
		 3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10,
		 2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10,
		 2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,
		 1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,
		 1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8}));
}

TEST_CASE("PartialPixelTest - Steep slopes - SLOPE_STEEP_S")
{
	CHECK(CheckPartialPixelZ(SLOPE_STEEP_S, {
		 0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,
		 1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,
		 1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,
		 2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9,
		 2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10,
		 3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10,
		 3,  4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11,
		 4,  4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11,
		 4,  5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12,
		 5,  5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12,
		 5,  6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13,
		 6,  6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13,
		 6,  7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14,
		 7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
		 7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15,
		 8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15}));
}

TEST_CASE("PartialPixelTest - Steep slopes - SLOPE_STEEP_W")
{
	CHECK(CheckPartialPixelZ(SLOPE_STEEP_W, {
		 8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,  0,
		 8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,
		 9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,
		 9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,
		10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,
		10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,
		11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,
		11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,  4,
		12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,  4,
		12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,  5,
		13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,  5,
		13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,  6,
		14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,  6,
		14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,  7,
		15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8,  7,
		15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10,  9,  9,  8,  8}));
}

TEST_CASE("PartialPixelTest - Half tile on top of steep slopes - SLOPE_STEEP/CORNER_N")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_STEEP_N, CORNER_N), {
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,
		16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,
		16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,
		16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,
		16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,
		16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,
		16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,
		16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,
		16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,
		16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1}));
}

TEST_CASE("PartialPixelTest - Half tile on top of steep slopes - SLOPE_STEEP/CORNER_E")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_STEEP_E, CORNER_E), {
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16,
		 4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16,
		 3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16,
		 3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16,
		 2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16,
		 2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16,
		 1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16,
		 1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16}));
}

TEST_CASE("PartialPixelTest - Half tile on top of steep slopes - SLOPE_STEEP/CORNER_S")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_STEEP_S, CORNER_S), {
		 0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8,
		 1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16,
		 1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16,
		 2,  2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16,
		 2,  3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16,
		 3,  3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16,
		 3,  4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16,
		 4,  4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16,
		 4,  5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16,
		 5,  5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 5,  6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 6,  6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 6,  7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 7,  7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 7,  8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
		 8, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16}));
}

TEST_CASE("PartialPixelTest - Half tile on top of steep slopes - SLOPE_STEEP/CORNER_W")
{
	CHECK(CheckPartialPixelZ(HalftileSlope(SLOPE_STEEP_W, CORNER_W), {
		 8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,  0,
		16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,  1,
		16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,  1,
		16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,  2,
		16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,  2,
		16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,  3,
		16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,  3,
		16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,  4,
		16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,  4,
		16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,  5,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,  5,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,  6,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,  6,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,  7,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8,  7,
		16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  8}));
}
