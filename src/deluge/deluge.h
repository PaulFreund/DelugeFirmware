/*
 * Copyright © 2023 Synthstrom Audible Limited
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
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int main(void);
extern int32_t deluge_main(void);

extern void timerGoneOff(void);

extern void routineWithClusterLoading(void);
extern void loadAnyEnqueuedClustersRoutine(void);

extern void logAudioAction(char const* string);

// This is defined in display.cpp
extern void freezeWithError(char const* errmsg);
extern void consoleTextIfAllBootedUp(char const* text);

extern void routineForSD(void);
extern void sdCardInserted(void);
extern void sdCardEjected(void);

extern void setTimeUSBInitializationEnds(int32_t timeFromNow);

// The following is for use by RZA1, based on gui/l10n/strings.h
// THIS MUST MATCH THE VALUES OF THESE ENTRIES IN deluge::l10n::String
enum l10n_string {
	STRING_FOR_USB_DEVICES_MAX = 256,
	STRING_FOR_USB_DEVICE_DETACHED,
	STRING_FOR_USB_HUB_ATTACHED,
	STRING_FOR_USB_DEVICE_NOT_RECOGNIZED,
};

char const* l10n_get(enum l10n_string s);

#ifdef __cplusplus
}
#endif
