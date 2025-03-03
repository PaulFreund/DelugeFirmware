#pragma once
#include "definitions_cxx.hpp"
#include <array>
#include <string_view>

class NumericLayer;
class NumericLayerScrollingText;

namespace deluge::hid {

class Display {
public:
	virtual ~Display() = default;

	constexpr virtual size_t getNumBrowserAndMenuLines() = 0;

	virtual void setText(std::string_view newText, bool alignRight = false, uint8_t drawDot = 255, bool doBlink = false,
	                     uint8_t* newBlinkMask = NULL, bool blinkImmediately = false, bool shouldBlinkFast = false,
	                     int32_t scrollPos = 0, uint8_t* blinkAddition = NULL, bool justReplaceBottomLayer = false){};

	virtual void displayPopup(char const* newText, int8_t numFlashes = 3, bool alignRight = false, uint8_t = 255,
	                          int32_t = 1) = 0;

	virtual void popupText(char const* text) = 0;
	virtual void popupTextTemporary(char const* text) = 0;

	virtual void setNextTransitionDirection(int8_t thisDirection){};

	virtual void cancelPopup() = 0;
	virtual void freezeWithError(char const* text) = 0;
	virtual bool isLayerCurrentlyOnTop(NumericLayer* layer) = 0;
	virtual void displayError(int32_t error) = 0;

	virtual void removeWorkingAnimation() = 0;

	// Loading animations
	virtual void displayLoadingAnimation(){};
	virtual void displayLoadingAnimationText(char const* text, bool delayed = false, bool transparent = false) = 0;
	virtual void removeLoadingAnimation() = 0;

	virtual bool hasPopup() = 0;

	virtual void consoleText(char const* text) = 0;

	virtual void timerRoutine() = 0;

	virtual void setTextAsNumber(int16_t number, uint8_t drawDot = 255, bool doBlink = false) {}
	virtual int32_t getEncodedPosFromLeft(int32_t textPos, char const* text, bool* andAHalf) { return 0; }
	virtual void setTextAsSlot(int16_t currentSlot, int8_t currentSubSlot, bool currentSlotExists, bool doBlink = false,
	                           int32_t blinkPos = -1, bool blinkImmediately = false) {}
	virtual NumericLayerScrollingText* setScrollingText(char const* newText, int32_t startAtPos = 0,
	                                                    int32_t initialDelay = 600) {
		return nullptr;
	}

	virtual std::array<uint8_t, kNumericDisplayLength> getLast() { return {0}; }; // to match SevenSegment

	virtual bool haveOLED() { return false; }
	virtual bool have7SEG() { return false; }
};

} // namespace deluge::hid

extern deluge::hid::Display* display;

extern "C" void consoleTextIfAllBootedUp(char const* text);
