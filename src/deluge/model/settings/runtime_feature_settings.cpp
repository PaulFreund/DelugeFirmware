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

#include "runtime_feature_settings.h"
#include "gui/l10n/l10n.h"
#include "hid/display/display.h"
#include "storage/storage_manager.h"
#include "util/d_string.h"
#include <cstdio>
#include <cstring>
#include <new>

#define RUNTIME_FEATURE_SETTINGS_FILE "CommunityFeatures.XML"
#define TAG_RUNTIME_FEATURE_SETTINGS "runtimeFeatureSettings"
#define TAG_RUNTIME_FEATURE_SETTING "setting"
#define TAG_RUNTIME_FEATURE_SETTING_ATTR_NAME "name"
#define TAG_RUNTIME_FEATURE_SETTING_ATTR_VALUE "value"

/// Unknown Settings container
struct UnknownSetting {
	std::string name;
	uint32_t value;
};

RuntimeFeatureSettings runtimeFeatureSettings{};

RuntimeFeatureSettings::RuntimeFeatureSettings() : unknownSettings(sizeof(UnknownSetting)) {
}

static void SetupOnOffSetting(RuntimeFeatureSetting& setting, const std::string& displayName,
                              const std::string& xmlName, RuntimeFeatureStateToggle def) {
	setting.displayName = displayName;
	setting.xmlName = xmlName;
	setting.value = static_cast<uint32_t>(def);

	setting.options = {
	    {
	        .displayName = "Off",
	        .value = RuntimeFeatureStateToggle::Off,
	    },
	    {
	        .displayName = "On",
	        .value = RuntimeFeatureStateToggle::On,
	    },
	};
}

static void SetupSyncScalingActionSetting(RuntimeFeatureSetting& setting, char const* const displayName,
                                          char const* const xmlName, RuntimeFeatureStateSyncScalingAction def) {
	setting.displayName = displayName;
	setting.xmlName = xmlName;
	setting.value = static_cast<uint32_t>(def);

	setting.options = {{
	                       .displayName = display->haveOLED() ? "Sync Scaling" : "SCAL",
	                       .value = RuntimeFeatureStateSyncScalingAction::SyncScaling,
	                   },
	                   {
	                       .displayName = display->haveOLED() ? "Fill mode" : "FILL",
	                       .value = RuntimeFeatureStateSyncScalingAction::Fill,
	                   }};
}

void RuntimeFeatureSettings::init() {
	// Drum randomizer
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::DrumRandomizer],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_DRUM_RANDOMIZER),
	                  "drumRandomizer", RuntimeFeatureStateToggle::On);
	// Master compressor
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::MasterCompressorFx],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_MASTER_COMPRESSOR),
	                  "masterCompressor", RuntimeFeatureStateToggle::On);
	// Quantize
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::Quantize],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_QUANTIZE), "quantize",
	                  RuntimeFeatureStateToggle::On);
	// FineTempoKnob
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::FineTempoKnob],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_FINE_TEMPO_KNOB),
	                  "fineTempoKnob", RuntimeFeatureStateToggle::On);
	// PatchCableResolution
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::PatchCableResolution],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_MOD_DEPTH_DECIMALS),
	                  "modDepthDecimals", RuntimeFeatureStateToggle::On);
	// CatchNotes
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::CatchNotes],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_CATCH_NOTES), "catchNotes",
	                  RuntimeFeatureStateToggle::On);
	// DeleteUnusedKitRows
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::DeleteUnusedKitRows],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_DELETE_UNUSED_KIT_ROWS),
	                  "deleteUnusedKitRows", RuntimeFeatureStateToggle::On);
	// AltGoldenKnobDelayParams
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::AltGoldenKnobDelayParams],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_ALT_DELAY_PARAMS),
	                  "altGoldenKnobDelayParams", RuntimeFeatureStateToggle::Off);
	// QuantizedStutterRate
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::QuantizedStutterRate],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_QUANTIZED_STUTTER),
	                  "quantizedStutterRate", RuntimeFeatureStateToggle::Off);
	// InterpolateAutomation
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::AutomationInterpolate],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_INTERPOLATION),
	                  "automationInterpolate", RuntimeFeatureStateToggle::On);
	// ClearClipAutomation
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::AutomationClearClip],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_CLEAR_CLIP),
	                  "automationClearClip", RuntimeFeatureStateToggle::On);
	// NudgeNoteAutomation
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::AutomationNudgeNote],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_NUDGE_NOTE),
	                  "automationNudgeNote", RuntimeFeatureStateToggle::On);
	// ShiftNoteAutomation
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::AutomationShiftClip],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_AUTOMATION_SHIFT_CLIP),
	                  "automationShiftClip", RuntimeFeatureStateToggle::On);
	// devSysexAllowed
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::DevSysexAllowed],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_DEV_SYSEX),
	                  "devSysexAllowed", RuntimeFeatureStateToggle::Off);
	// SyncScalingAction
	SetupSyncScalingActionSetting(
	    settings[RuntimeFeatureSettingType::SyncScalingAction],
	    deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_SYNC_SCALING_ACTION), "syncScalingAction",
	    RuntimeFeatureStateSyncScalingAction::SyncScaling);
	// HighlightIncomingNotes
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::HighlightIncomingNotes],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_HIGHLIGHT_INCOMING_NOTES),
	                  "highlightIncomingNotes", RuntimeFeatureStateToggle::On);
	// DisplayNornsLayout
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::DisplayNornsLayout],
	                  deluge::l10n::get(deluge::l10n::String::STRING_FOR_COMMUNITY_FEATURE_NORNS_LAYOUT),
	                  "displayNornsLayout", RuntimeFeatureStateToggle::Off);

	// ShiftIsSticky
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::ShiftIsSticky], "Sticky Shift", "stickyShift",
	                  RuntimeFeatureStateToggle::Off);

	// LightShiftLed
	SetupOnOffSetting(settings[RuntimeFeatureSettingType::LightShiftLed], "Light Shift", "lightShift",
	                  RuntimeFeatureStateToggle::Off);
}

void RuntimeFeatureSettings::readSettingsFromFile() {
	FilePointer fp;
	bool success = storageManager.fileExists(RUNTIME_FEATURE_SETTINGS_FILE, &fp);
	if (!success) {
		return;
	}

	int32_t error = storageManager.openXMLFile(&fp, TAG_RUNTIME_FEATURE_SETTINGS);
	if (error) {
		return;
	}

	String currentName;
	int32_t currentValue = 0;
	char const* currentTag = nullptr;
	while (*(currentTag = storageManager.readNextTagOrAttributeName())) {
		if (strcmp(currentTag, TAG_RUNTIME_FEATURE_SETTING) == 0) {
			// Read name
			currentTag = storageManager.readNextTagOrAttributeName();
			if (strcmp(currentTag, TAG_RUNTIME_FEATURE_SETTING_ATTR_NAME) != 0) {
				display->displayPopup("Community file err");
				break;
			}
			storageManager.readTagOrAttributeValueString(&currentName);
			storageManager.exitTag();

			// Read value
			currentTag = storageManager.readNextTagOrAttributeName();
			if (strcmp(currentTag, TAG_RUNTIME_FEATURE_SETTING_ATTR_VALUE) != 0) {
				display->displayPopup("Community file err");
				break;
			}
			currentValue = storageManager.readTagOrAttributeValueInt();
			storageManager.exitTag();

			bool found = false;
			for (auto& setting : settings) {
				if (strcmp(setting.xmlName.c_str(), currentName.get()) == 0) {
					found = true;
					setting.value = currentValue;
				}
			}

			// Remember unknown settings for writing them back
			if (!found) {
				//unknownSettings.insertSetting(&currentName, currentValue);
				int32_t idx = unknownSettings.getNumElements();
				if (unknownSettings.insertAtIndex(idx) != NO_ERROR) {
					return;
				}
				void* address = unknownSettings.getElementAddress(idx);
				auto* unknownSetting = new (address) UnknownSetting();
				unknownSetting->name = std::string(currentName.get());
				unknownSetting->value = currentValue;
			}
		}

		storageManager.exitTag();
	}

	storageManager.closeFile();
}

void RuntimeFeatureSettings::writeSettingsToFile() {
	f_unlink(RUNTIME_FEATURE_SETTINGS_FILE); // May give error, but no real consequence from that.

	int32_t error = storageManager.createXMLFile(RUNTIME_FEATURE_SETTINGS_FILE, true);
	if (error) {
		return;
	}

	storageManager.writeOpeningTagBeginning(TAG_RUNTIME_FEATURE_SETTINGS);
	storageManager.writeFirmwareVersion();
	storageManager.writeEarliestCompatibleFirmwareVersion("4.1.3");
	storageManager.writeOpeningTagEnd();

	for (auto& setting : settings) {
		storageManager.writeOpeningTagBeginning(TAG_RUNTIME_FEATURE_SETTING);
		storageManager.writeAttribute(TAG_RUNTIME_FEATURE_SETTING_ATTR_NAME, setting.xmlName.c_str(), false);
		storageManager.writeAttribute(TAG_RUNTIME_FEATURE_SETTING_ATTR_VALUE, setting.value, false);
		storageManager.writeOpeningTagEnd(false);
		storageManager.writeClosingTag(TAG_RUNTIME_FEATURE_SETTING, false);
	}

	// Write unknown elements
	for (uint32_t idxUnknownSetting = 0; idxUnknownSetting < unknownSettings.getNumElements(); idxUnknownSetting++) {
		UnknownSetting* unknownSetting = (UnknownSetting*)unknownSettings.getElementAddress(idxUnknownSetting);
		storageManager.writeOpeningTagBeginning(TAG_RUNTIME_FEATURE_SETTING);
		storageManager.writeAttribute(TAG_RUNTIME_FEATURE_SETTING_ATTR_NAME, unknownSetting->name.c_str(), false);
		storageManager.writeAttribute(TAG_RUNTIME_FEATURE_SETTING_ATTR_VALUE, unknownSetting->value, false);
		storageManager.writeOpeningTagEnd(false);
		storageManager.writeClosingTag(TAG_RUNTIME_FEATURE_SETTING, false);
	}

	storageManager.writeClosingTag(TAG_RUNTIME_FEATURE_SETTINGS);
	storageManager.closeFileAfterWriting();
}
