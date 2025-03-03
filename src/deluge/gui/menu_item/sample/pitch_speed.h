/*
 * Copyright (c) 2014-2023 Synthstrom Audible Limited
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
#include "gui/menu_item/selection.h"
#include "gui/ui/sound_editor.h"
#include "model/clip/clip.h"
#include "model/drum/drum.h"
#include "model/drum/kit.h"
#include "model/song/song.h"
#include "processing/sound/sound_drum.h"

namespace deluge::gui::menu_item::sample {
class PitchSpeed final : public Selection<2> {
public:
	using Selection::Selection;

	bool usesAffectEntire() override { return true; }

	void readCurrentValue() override { this->setValue(soundEditor.currentSampleControls->pitchAndSpeedAreIndependent); }

	void writeCurrentValue() override {
		// If affect-entire button held, do whole kit
		if (currentUIMode == UI_MODE_HOLDING_AFFECT_ENTIRE_IN_SOUND_EDITOR && soundEditor.editingKit()) {

			Kit* kit = static_cast<Kit*>(currentSong->currentClip->output);

			for (Drum* thisDrum = kit->firstDrum; thisDrum != nullptr; thisDrum = thisDrum->next) {
				if (thisDrum->type == DrumType::SOUND) {
					auto* soundDrum = static_cast<SoundDrum*>(thisDrum);
					Source* source = &soundDrum->sources[soundEditor.currentSourceIndex];

					source->sampleControls.pitchAndSpeedAreIndependent = this->getValue();
				}
			}
		}

		// Or, the normal case of just one sound
		else {
			soundEditor.currentSampleControls->pitchAndSpeedAreIndependent = this->getValue();
		}
	}

	static_vector<std::string_view, capacity()> getOptions() override {
		return {l10n::getView(l10n::String::STRING_FOR_LINKED), l10n::getView(l10n::String::STRING_FOR_INDEPENDENT)};
	}
};
} // namespace deluge::gui::menu_item::sample
