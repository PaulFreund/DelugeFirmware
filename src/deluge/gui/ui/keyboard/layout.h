/*
 * Copyright © 2016-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "hid/button.h"
#include <string.h>
#include <array>

#define MAX_NUM_KEYBOARD_PAD_PRESSES 10

namespace keyboard {

struct KeyboardPadPress {
	uint8_t x;
	uint8_t y;
};

class KeyboardLayout {
public:
	KeyboardLayout() {}
	virtual ~KeyboardLayout() {}

	// Handle inputs
	virtual void handlePad(int x, int y, int velocity) = 0;
	virtual void handleSidebarPad(int x, int y, int velocity) = 0;
	virtual bool handleVerticalEncoder(int offset) = 0; // returns weather the scroll had an effect
	virtual bool handleHorizontalEncoder(int offset) = 0; // returns weather the scroll had an effect

	// Handle output
	virtual void renderPads(uint8_t image[][displayWidth + sideBarWidth][3]) {

	}
	virtual void renderSidebarPads(uint8_t image[][displayWidth + sideBarWidth][3]) {
		// Empty if not implemented
		for (int y = 0; y < displayHeight; y++) {
			memset(image[y][displayWidth], 0, sideBarWidth * 3);
		}
	};

	// Properties
	virtual bool supportsInstrument() { return false; }
	virtual bool supportsKit() { return false; }

	//@TODO: This also needs velocity
	//virtual std::array<int, MAX_NUM_KEYBOARD_PAD_PRESSES> getActiveNotes() = 0;

	//@TODO: rootNote(), scale()
};

}; // namespace keyboard
