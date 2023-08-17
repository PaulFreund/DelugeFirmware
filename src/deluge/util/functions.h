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
#include "fatfs/ff.h"
#include "util/fixedpoint.h"
#include "util/lookuptables/lookuptables.h"
#include <cmath>
#include <cstdint>
#include <cstring>
extern "C" {
#include "util/cfunctions.h"
}

class UI;

extern UI* getCurrentUI();

extern const uint8_t modButtonX[];
extern const uint8_t modButtonY[];

extern uint8_t subModeToReturnTo;

extern int32_t paramRanges[];
extern int32_t paramNeutralValues[];

void functionsInit();

static inline void intToString(int32_t number, char* buffer) {
	intToString(number, buffer, 1);
}

bool memIsNumericChars(char const* mem, int32_t size);
bool stringIsNumericChars(char const* str);
char const* getThingName(InstrumentType instrumentType);

char halfByteToHexChar(uint8_t thisHalfByte);
void intToHex(uint32_t number, char* output, int32_t numChars = 8);
uint32_t hexToInt(char const* string);
uint32_t hexToIntFixedLength(char const* __restrict__ hexChars, int32_t length);

void byteToHex(uint8_t number, char* buffer);
uint8_t hexToByte(char const* firstChar);

static inline int32_t add_saturation(int32_t a, int32_t b) __attribute__((always_inline, unused));
static inline int32_t add_saturation(int32_t a, int32_t b) {
	int32_t out;
	asm("qadd %0, %1, %2" : "=r"(out) : "r"(a), "r"(b));
	return out;
}

// computes limit((val >> rshift), 2**bits)
template <uint8_t bits>
static inline int32_t signed_saturate(int32_t val) __attribute__((always_inline, unused));
template <uint8_t bits>
static inline int32_t signed_saturate(int32_t val) {
	int32_t out;
	asm("ssat %0, %1, %2" : "=r"(out) : "I"(bits), "r"(val));
	return out;
}

// bits must be *less* than 32! I.e. 31 or less
inline int32_t signed_saturate_operand_unknown(int32_t val, int32_t bits) {

	// Despite having this switch at the per-audio-sample level, it doesn't introduce any slowdown compared to just always saturating by the same amount!
	switch (bits) {
	case 31:
		return signed_saturate<31>(val);
	case 30:
		return signed_saturate<30>(val);
	case 29:
		return signed_saturate<29>(val);
	case 28:
		return signed_saturate<28>(val);
	case 27:
		return signed_saturate<27>(val);
	case 26:
		return signed_saturate<26>(val);
	case 25:
		return signed_saturate<25>(val);
	case 24:
		return signed_saturate<24>(val);
	case 23:
		return signed_saturate<23>(val);
	case 22:
		return signed_saturate<22>(val);
	case 21:
		return signed_saturate<21>(val);
	case 20:
		return signed_saturate<20>(val);
	case 19:
		return signed_saturate<19>(val);
	case 18:
		return signed_saturate<18>(val);
	case 17:
		return signed_saturate<17>(val);
	case 16:
		return signed_saturate<16>(val);
	case 15:
		return signed_saturate<15>(val);
	case 14:
		return signed_saturate<14>(val);
	case 13:
		return signed_saturate<13>(val);
	default:
		return signed_saturate<12>(val);
	}
}

template <uint8_t lshift>
inline int32_t lshiftAndSaturate(int32_t val) {
	return signed_saturate<32 - lshift>(val) << lshift;
}

// lshift must be greater than 0! Not 0
inline int32_t lshiftAndSaturateUnknown(int32_t val, uint8_t lshift) {
	return signed_saturate_operand_unknown(val, 32 - lshift) << lshift;
}

static constexpr uint32_t charsToIntegerConstant(char a, char b, char c, char d) {
	return (static_cast<uint32_t>(a)) | (static_cast<uint32_t>(b) << 8) | (static_cast<uint32_t>(c) << 16)
	       | (static_cast<uint32_t>(d) << 24);
}

static constexpr uint16_t charsToIntegerConstant(char a, char b) {
	return (static_cast<uint16_t>(a)) | (static_cast<uint16_t>(b) << 8);
}

int32_t stringToInt(char const* string);
int32_t stringToUIntOrError(char const* mem);
int32_t memToUIntOrError(char const* mem, char const* const memEnd);
void getInstrumentPresetFilename(char const* filePrefix, int16_t presetNumber, int8_t presetSubslotNumber,
                                 char* fileName);
char const* oscTypeToString(OscType osctype);
OscType stringToOscType(char const* string);

char const* lfoTypeToString(LFOType oscType);
LFOType stringToLFOType(char const* string);

char const* synthModeToString(SynthMode synthMode);
SynthMode stringToSynthMode(char const* string);

char const* polyphonyModeToString(PolyphonyMode synthMode);
PolyphonyMode stringToPolyphonyMode(char const* string);

char const* fxTypeToString(ModFXType fxType);
ModFXType stringToFXType(char const* string);

char const* modFXParamToString(ModFXParam fxType);
ModFXParam stringToModFXParam(char const* string);

char const* filterTypeToString(FilterType fxType);
FilterType stringToFilterType(char const* string);

FilterRoute stringToFilterRoute(char const* string);
char const* filterRouteToString(FilterRoute route);

char const* arpModeToString(ArpMode mode);
ArpMode stringToArpMode(char const* string);

char const* lpfTypeToString(FilterMode lpfType);
FilterMode stringToLPFType(char const* string);

char const* inputChannelToString(AudioInputChannel inputChannel);
AudioInputChannel stringToInputChannel(char const* string);

char const* sequenceDirectionModeToString(SequenceDirection sequenceDirectionMode);
SequenceDirection stringToSequenceDirectionMode(char const* string);

char const* getInstrumentFolder(InstrumentType instrumentType);
void getThingFilename(char const* thingName, int16_t currentSlot, int8_t currentSubSlot, char* buffer);

int32_t getExp(int32_t presetValue, int32_t adjustment);

bool isAudioFilename(char const* filename);
bool isAiffFilename(char const* filename);

char const* getFileNameFromEndOfPath(char const* filePathChars);

int32_t lookupReleaseRate(int32_t input);
int32_t getParamFromUserValue(uint8_t p, int8_t userValue);
int32_t getLookupIndexFromValue(int32_t value, const int32_t* table, int32_t maxIndex);
uint32_t rshift_round(uint32_t value, uint32_t rshift);
int32_t rshift_round_signed(int32_t value, uint32_t rshift);
int32_t instantTan(int32_t input);

int32_t combineHitStrengths(int32_t strength1, int32_t strength2);

int32_t cableToLinearParamShortcut(int32_t sourceValue);
int32_t cableToExpParamShortcut(int32_t sourceValue);

class Sound;
class StereoSample;
int32_t getFinalParameterValueVolume(int32_t paramNeutralValue, int32_t patchedValue);
int32_t getFinalParameterValueLinear(int32_t paramNeutralValue, int32_t patchedValue);
int32_t getFinalParameterValueHybrid(int32_t paramNeutralValue, int32_t patchedValue);
int32_t getFinalParameterValueExp(int32_t paramNeutralValue, int32_t patchedValue);
int32_t getFinalParameterValueExpWithDumbEnvelopeHack(int32_t paramNeutralValue, int32_t patchedValue, int32_t p);

void addAudio(StereoSample* inputBuffer, StereoSample* outputBuffer, int32_t numSamples);

#if HAVE_OLED
char const* getSourceDisplayNameForOLED(PatchSource s);
char const* getPatchedParamDisplayNameForOLED(int32_t p);
char const* getUnpatchedParamDisplayNameForOLED(int32_t p);
char const* getGlobalEffectableParamDisplayNameForOLED(int32_t p);
#endif

char const* sourceToString(PatchSource source);
PatchSource stringToSource(char const* string);
char const* sourceToStringShort(PatchSource source);

char const* patchedParamToStringShort(int32_t p);

bool paramNeedsLPF(int32_t p, bool fromAutomation);
int32_t shiftVolumeByDB(int32_t oldValue, float offset);
int32_t quickLog(uint32_t input);

static void convertFloatToIntAtMemoryLocation(uint32_t* pos) {

	//*(int32_t*)pos = *(float*)pos * 2147483648;

	uint32_t readValue = *(uint32_t*)pos;
	int32_t exponent = (int32_t)((readValue >> 23) & 255) - 127;

	int32_t outputValue = (exponent >= 0) ? 2147483647 : (uint32_t)((readValue << 8) | 0x80000000) >> (-exponent);

	// Sign bit
	if (readValue >> 31)
		outputValue = -outputValue;

	*pos = outputValue;
}

static int32_t floatToInt(float theFloat) {
	uint32_t readValue = *(uint32_t*)&theFloat;
	int32_t exponent = (int32_t)((readValue >> 23) & 255) - 127;

	int32_t outputValue = (exponent >= 0) ? 2147483647 : (uint32_t)((readValue << 8) | 0x80000000) >> (-exponent);

	// Sign bit
	if (readValue >> 31)
		outputValue = -outputValue;

	return outputValue;
}

static int32_t floatBitPatternToInt(uint32_t readValue) {
	int32_t exponent = (int32_t)((readValue >> 23) & 255) - 127;

	int32_t outputValue = (exponent >= 0) ? 2147483647 : (uint32_t)((readValue << 8) | 0x80000000) >> (-exponent);

	// Sign bit
	if (readValue >> 31)
		outputValue = -outputValue;

	return outputValue;
}

// input must not have any extra bits set than numBitsInInput specifies
inline int32_t interpolateTableSigned(uint32_t input, int32_t numBitsInInput, const int16_t* table,
                                      int32_t numBitsInTableSize = 8) {
	int32_t whichValue = input >> (numBitsInInput - numBitsInTableSize);
	int32_t rshiftAmount = numBitsInInput - 16 - numBitsInTableSize;
	uint32_t rshifted;
	if (rshiftAmount >= 0)
		rshifted = input >> rshiftAmount;
	else
		rshifted = input << (-rshiftAmount);
	int32_t strength2 = rshifted & 65535;
	int32_t strength1 = 65536 - strength2;
	return (int32_t)table[whichValue] * strength1 + (int32_t)table[whichValue + 1] * strength2;
}

int32_t interpolateTable(uint32_t input, int32_t numBitsInInput, const uint16_t* table, int32_t numBitsInTableSize = 8);
uint32_t interpolateTableInverse(int32_t tableValueBig, int32_t numBitsInLookupOutput, const uint16_t* table,
                                 int32_t numBitsInTableSize = 8);

// input must not have any extra bits set than numBitsInInput specifies
// Output of this function (unlike the regular 1d one) is only +- 1073741824
inline int32_t interpolateTableSigned2d(uint32_t inputX, uint32_t inputY, int32_t numBitsInInputX,
                                        int32_t numBitsInInputY, const int16_t* table, int32_t numBitsInTableSizeX,
                                        int32_t numBitsInTableSizeY) {

	int32_t whichValue = inputY >> (numBitsInInputY - numBitsInTableSizeY);

	int32_t tableSizeOneRow = (1 << numBitsInTableSizeX) + 1;

	int32_t value1 =
	    interpolateTableSigned(inputX, numBitsInInputX, &table[whichValue * tableSizeOneRow], numBitsInTableSizeX);
	int32_t value2 = interpolateTableSigned(inputX, numBitsInInputX, &table[(whichValue + 1) * tableSizeOneRow],
	                                        numBitsInTableSizeX);

	int32_t lshiftAmount = 31 + numBitsInTableSizeY - numBitsInInputY;

	uint32_t strength2;

	if (lshiftAmount >= 0)
		strength2 = (inputY << lshiftAmount) & 2147483647;
	else
		strength2 = (inputY >> (0 - lshiftAmount)) & 2147483647;

	uint32_t strength1 = 2147483647 - strength2;
	return multiply_32x32_rshift32(value1, strength1) + multiply_32x32_rshift32(value2, strength2);
}

template <unsigned saturationAmount>
inline int32_t getTanH(int32_t input) {
	uint32_t workingValue;

	if (saturationAmount)
		workingValue = (uint32_t)lshiftAndSaturate<saturationAmount>(input) + 2147483648u;
	else
		workingValue = (uint32_t)input + 2147483648u;

	return interpolateTableSigned(workingValue, 32, tanHSmall, 8) >> (saturationAmount + 2);
}

inline int32_t getTanHUnknown(int32_t input, uint32_t saturationAmount) {
	uint32_t workingValue;

	if (saturationAmount)
		workingValue = (uint32_t)lshiftAndSaturateUnknown(input, saturationAmount) + 2147483648u;
	else
		workingValue = (uint32_t)input + 2147483648u;

	return interpolateTableSigned(workingValue, 32, tanHSmall, 8) >> (saturationAmount + 2);
}

inline int32_t getTanHAntialiased(int32_t input, uint32_t* lastWorkingValue, uint32_t saturationAmount) {

	uint32_t workingValue = (uint32_t)lshiftAndSaturateUnknown(input, saturationAmount) + 2147483648u;

	int32_t toReturn = interpolateTableSigned2d(workingValue, *lastWorkingValue, 32, 32, &tanH2d[0][0], 7, 6)
	                   >> (saturationAmount + 1);
	*lastWorkingValue = workingValue;
	return toReturn;
}

inline int32_t getSine(uint32_t phase, uint8_t numBitsInInput = 32) {
	return interpolateTableSigned(phase, numBitsInInput, sineWaveSmall, 8);
}

inline int32_t getSquare(uint32_t phase, uint32_t phaseWidth = 2147483648u) {
	return ((phase >= phaseWidth) ? (-2147483648) : 2147483647);
}

inline int32_t getSquareSmall(uint32_t phase, uint32_t phaseWidth = 2147483648u) {
	return ((phase >= phaseWidth) ? (-1073741824) : 1073741823);
}

/* Should be faster, but isn't...
 * inline int32_t getTriangleSmall(int32_t phase) {
	int32_t multiplier = (phase >> 31) | 1;
	phase *= multiplier;
 */

inline int32_t getTriangleSmall(uint32_t phase) {
	if (phase >= 2147483648u)
		phase = -phase;
	return phase - 1073741824;
}

inline int32_t getTriangle(uint32_t phase) {
	return ((phase < 2147483648u) ? 2 : -2) * phase + 2147483648u;
	//return getTriangleSmall(phase) << 1;
}

int32_t getDecay8(uint32_t input, uint8_t numBitsInInput);
int32_t getDecay4(uint32_t input, uint8_t numBitsInInput);

#define znew (z = 36969 * (z & 65535) + (z >> 16))
#define wnew (w = 18000 * (w & 65535) + (w >> 16))
#define MWC (int32_t)((znew << 16) + wnew)
#define CONG (jcong = 69069 * jcong + 1234567)

extern uint32_t z, w, jcong;

inline uint8_t getRandom255() {
	return CONG >> 24;
}

inline int32_t getNoise() {
	return CONG;
}

void seedRandom();

extern bool shouldInterpretNoteNames;
extern bool octaveStartsFromA;

int32_t random(int32_t upperLimit);
bool shouldDoPanning(int32_t panAmount, int32_t* amplitudeL, int32_t* amplitudeR);
void hueToRGB(int32_t hue, unsigned char* rgb);
void hueToRGBPastel(int32_t hue, unsigned char* rgb);

uint32_t getLFOInitialPhaseForNegativeExtreme(LFOType waveType);
uint32_t getLFOInitialPhaseForZero(LFOType waveType);

uint32_t getOscInitialPhaseForZero(OscType waveType);
int32_t fastPythag(int32_t x, int32_t y);
int32_t strcmpspecial(char const* first, char const* second);
int32_t doLanczos(int32_t* data, int32_t pos, uint32_t posWithinPos, int32_t memoryNumElements);
int32_t doLanczosCircular(int32_t* data, int32_t pos, uint32_t posWithinPos, int32_t memoryNumElements);
int32_t stringToFirmwareVersion(char const* firmwareVersionString);

// intensity is out of 65536 now
// occupancyMask is out of 64 now
inline void drawSquare(uint8_t squareColour[], int32_t intensity, uint8_t square[], uint8_t* occupancyMask,
                       int32_t occupancyFromWhichColourCame) {

	int32_t modifiedIntensity = intensity; //(intensity * occupancyFromWhichColourCame) >> 6; // Out of 65536

	// Make new colour being drawn into this square marginalise the colour already in the square
	int32_t colourRemainingAmount = 65536;

	// We know how much colour we want to add to this square, so constrain any existing colour to the remaining "space" that it may still occupy
	int32_t maxOldOccupancy = (65536 - modifiedIntensity) >> 10;

	// If the square has more colour in it than it's allowed to retain, then plan to reduce it
	if (*occupancyMask > maxOldOccupancy)
		colourRemainingAmount = (maxOldOccupancy << 16) / *occupancyMask; // out of 65536

	// Add the new colour, reducing the old if that's what we're doing
	int32_t newOccupancyMaskValue =
	    rshift_round(*occupancyMask * colourRemainingAmount, 16) + rshift_round(modifiedIntensity, 10);
	*occupancyMask = std::min<int32_t>(64, newOccupancyMaskValue);

	for (int32_t colour = 0; colour < 3; colour++) {
		int32_t newColourValue = rshift_round((int32_t)square[colour] * colourRemainingAmount, 16)
		                         + rshift_round((int32_t)squareColour[colour] * modifiedIntensity, 16);
		square[colour] = std::min<int32_t>(255, newColourValue);
	}
}

inline void drawSolidSquare(uint8_t squareColour[], uint8_t square[], uint8_t* occupancyMask) {
	memcpy(square, squareColour, 3);
	*occupancyMask = 64;
}

inline void getTailColour(uint8_t rgb[], uint8_t fromRgb[]) {
	uint32_t averageBrightness = ((uint32_t)fromRgb[0] + fromRgb[1] + fromRgb[2]);
	rgb[0] = (((int32_t)fromRgb[0] * 21 + averageBrightness) * 157) >> 14;
	rgb[1] = (((int32_t)fromRgb[1] * 21 + averageBrightness) * 157) >> 14;
	rgb[2] = (((int32_t)fromRgb[2] * 21 + averageBrightness) * 157) >> 14;
}

inline void getBlurColour(uint8_t rgb[], uint8_t fromRgb[]) {
	uint32_t averageBrightness = (uint32_t)fromRgb[0] * 5 + fromRgb[1] * 9 + fromRgb[2] * 9;
	rgb[0] = ((uint32_t)fromRgb[0] * 5 + averageBrightness) >> 5;
	rgb[1] = ((uint32_t)fromRgb[1] * 5 + averageBrightness) >> 5;
	rgb[2] = ((uint32_t)fromRgb[2] * 1 + averageBrightness) >> 5;
}

inline void colorCopy(uint8_t* dest, uint8_t* src, uint8_t intensity, uint8_t brightnessDivider) {
	dest[0] = (uint8_t)((src[0] * intensity / 255) / brightnessDivider);
	dest[1] = (uint8_t)((src[1] * intensity / 255) / brightnessDivider);
	dest[2] = (uint8_t)((src[2] * intensity / 255) / brightnessDivider);
}

inline int32_t increaseMagnitude(int32_t number, int32_t magnitude) {
	if (magnitude >= 0)
		return number << magnitude;
	else
		return number >> (-magnitude);
}

inline int32_t increaseMagnitudeAndSaturate(int32_t number, int32_t magnitude) {
	if (magnitude > 0)
		return lshiftAndSaturateUnknown(number, magnitude);
	else
		return number >> (-magnitude);
}

int32_t howMuchMoreMagnitude(uint32_t to, uint32_t from);
void noteCodeToString(int32_t noteCode, char* buffer, int32_t* getLengthWithoutDot = NULL);
double ConvertFromIeeeExtended(unsigned char* bytes /* LCN */);
int32_t divide_round_negative(int32_t dividend, int32_t divisor);
void dissectIterationDependence(int32_t probability, int32_t* getDivisor, int32_t* getWhichIterationWithinDivisor);
int32_t encodeIterationDependence(int32_t divisor, int32_t iterationWithinDivisor);

inline uint32_t swapEndianness32(uint32_t input) {
	int32_t out;
	asm("rev %0, %1" : "=r"(out) : "r"(input));
	return out;
}

inline uint32_t swapEndianness2x16(uint32_t input) {
	int32_t out;
	asm("rev16 %0, %1" : "=r"(out) : "r"(input));
	return out;
}

inline int32_t clz(uint32_t input) {
	int32_t out;
	asm("clz %0, %1" : "=r"(out) : "r"(input));
	return out;
}

inline int32_t getMagnitudeOld(uint32_t input) {
	return 32 - clz(input);
}

inline int32_t getMagnitude(uint32_t input) {
	return 31 - clz(input);
}

inline bool isPowerOfTwo(uint32_t input) {
	return (input == 1 << getMagnitude(input));
}

int32_t getWhichKernel(int32_t phaseIncrement);

int32_t memcasecmp(char const* first, char const* second, int32_t size);
int32_t getHowManyCharsAreTheSame(char const* a, char const* b);
void greyColourOut(const uint8_t* input, uint8_t* output, int32_t greyProportion);
void dimColour(uint8_t colour[3]);
bool charCaseEqual(char firstChar, char secondChar);
bool shouldAbortLoading();
void getNoteLengthNameFromMagnitude(char* text, int32_t magnitude, bool clarifyPerColumn = false);
bool doesFilenameFitPrefixFormat(char const* fileName, char const* filePrefix, int32_t prefixLength);
int32_t fresultToDelugeErrorCode(FRESULT result);

inline void writeInt16(char** address, uint16_t number) {
	*(uint16_t*)*address = number;
	*address += 2;
}

inline void writeInt32(char** address, uint32_t number) {
	*(uint32_t*)*address = number;
	*address += 4;
}

extern char miscStringBuffer[];
extern char shortStringBuffer[];
