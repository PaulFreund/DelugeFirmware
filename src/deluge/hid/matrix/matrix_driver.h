/*
 * Copyright © 2014-2023 Synthstrom Audible Limited
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

#include "definitions_cxx.hpp"
#include "gui/waveform/waveform_render_data.h"
#include "pad.h"
#include <cstdint>

class AudioClip;

class MatrixDriver {
public:
	MatrixDriver();

	bool isPadPressed(int32_t x, int32_t y);

	ActionResult padAction(int32_t x, int32_t y, int32_t velocity);
	void noPressesHappening(bool inCardRoutine);
	bool isUserDoingBootloaderOverwriteAction();

	bool padStates[kDisplayWidth + kSideBarWidth][kDisplayHeight];
};

extern char* matrixDriverDisplayWritePos;

extern MatrixDriver matrixDriver;
