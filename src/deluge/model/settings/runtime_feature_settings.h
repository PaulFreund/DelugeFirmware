/*
 * Copyright © 2015-2023 Synthstrom Audible Limited
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

#include "util/container/array/resizeable_array.h"
#include "util/container/static_vector.hpp"

#include <cstdint>

namespace deluge::gui::menu_item::runtime_feature {
class Setting;
class Settings;
class DevSysexSetting;
} // namespace deluge::gui::menu_item::runtime_feature

// State declarations
enum RuntimeFeatureStateToggle : uint32_t { Off = 0, On = 1 };

// Declare additional enums for specific multi state settings (e.g. like RuntimeFeatureStateTrackLaunchStyle)
enum RuntimeFeatureStateSyncScalingAction : uint32_t { SyncScaling = 0, Fill = 1 };

/// Every setting needs to be delcared in here
enum RuntimeFeatureSettingType : uint32_t {
	DrumRandomizer,
	MasterCompressorFx,
	Quantize,
	FineTempoKnob,
	PatchCableResolution,
	CatchNotes,
	DeleteUnusedKitRows,
	AltGoldenKnobDelayParams,
	QuantizedStutterRate,
	AutomationClearClip,
	AutomationNudgeNote,
	AutomationShiftClip,
	AutomationInterpolate,
	DevSysexAllowed,
	SyncScalingAction,
	HighlightIncomingNotes,
	DisplayNornsLayout,
	ShiftIsSticky,
	LightShiftLed,
	MaxElement // Keep as boundary
};

/// Definition for selectable options
struct RuntimeFeatureSettingOption {
	std::string displayName;
	uint32_t value; // Value to be defined as typed Enum above
};

/// Every setting keeps its metadata and value in here
struct RuntimeFeatureSetting {
	std::string displayName;
	std::string xmlName;
	uint32_t value;

	// Limited to safe memory
	deluge::static_vector<RuntimeFeatureSettingOption, RUNTIME_FEATURE_SETTING_MAX_OPTIONS> options;
};

/// Encapsulating class
class RuntimeFeatureSettings {
public:
	RuntimeFeatureSettings();

	// Traded type safety for option values for code simplicity and size, use enum from above to compare
	inline uint32_t get(RuntimeFeatureSettingType type) { return settings[type].value; };

	/**
	 * Set a runtime feature setting.
	 *
	 * Make sure when you use this the settings are eventually written back to the SDCard!
	 */
	inline void set(RuntimeFeatureSettingType type, uint32_t value) { settings[type].value = value; }

	void init();
	void readSettingsFromFile();
	void writeSettingsToFile();

protected:
	std::array<RuntimeFeatureSetting, RuntimeFeatureSettingType::MaxElement> settings = {};

private:
	ResizeableArray unknownSettings;

public:
	friend class deluge::gui::menu_item::runtime_feature::Setting;
	friend class deluge::gui::menu_item::runtime_feature::Settings;
	friend class deluge::gui::menu_item::runtime_feature::DevSysexSetting;
};

/// Static instance for external access
extern RuntimeFeatureSettings runtimeFeatureSettings;
