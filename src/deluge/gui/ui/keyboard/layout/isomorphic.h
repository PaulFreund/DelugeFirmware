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

#include "gui/ui/keyboard/layout.h"

namespace keyboard::layout {

class KeyboardLayoutIsomorphic : public KeyboardLayout {
public:
	KeyboardLayoutIsomorphic();
	virtual ~KeyboardLayoutIsomorphic() {}

	virtual void handlePad(int x, int y, int velocity);
	virtual void handleSidebarPad(int x, int y, int velocity);
	virtual bool handleVerticalEncoder(int offset);
	virtual bool handleHorizontalEncoder(int offset, bool shiftEnabled);

	virtual void renderPads(uint8_t image[][displayWidth + sideBarWidth][3]);

	virtual bool supportsInstrument() { return true; }
	virtual bool supportsKit() { return false; }

	virtual void stopAllNotes();

private:
	int noteFromCoords(int x, int y);

};

}; // namespace keyboard::layout

