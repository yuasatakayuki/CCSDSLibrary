/*
 * CCSDSSpacePacketSecondaryHeader.hh
 *
 *  Created on: Jun 9, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKETSECONDARYHEADER_HH_
#define CCSDSSPACEPACKETSECONDARYHEADER_HH_

#include <bitset>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "CCSDSSpacePacketException.hh"

class CCSDSSpacePacketSecondaryHeaderType {
public:
	enum {
		ADUChannelIsNotUsed = 0x00, //0b
		ADUChannelIsUsed = 0x01, //1b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketADUSegmentFlag {
public:
	enum {
		ContinuationSegument = 0x00, //00b
		TheFirstSegment = 0x01, //01b
		TheLastSegment = 0x02, //10b
		UnsegmentedADU = 0x03, //11b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketSecondaryHeader {
private:
	uint8_t time[4];
	std::bitset<1> secondaryHeaderType;
	std::bitset<7> category;
	uint8_t aduCount;
	uint8_t aduChannelID;
	std::bitset<2> aduSegmentFlag;
	std::bitset<14> aduSegmentCount;

public:
	static const size_t SecondaryHeaderLengthWithoutADUChannel = 6;
	static const size_t SecondaryHeaderLengthWithADUChannel = 9;

public:
	/** Constructor.
	 */
	CCSDSSpacePacketSecondaryHeader() {
		this->time[0] = 0x00;
		this->time[1] = 0x00;
		this->time[2] = 0x00;
		this->time[3] = 0x00;
		this->secondaryHeaderType.reset();
		this->category.reset();
		this->aduCount = 0x00;
		this->aduChannelID = 0x00;
		this->aduSegmentFlag.reset();
		this->aduSegmentCount.reset();
	}

public:
	/** Destructor.
	 */
	~CCSDSSpacePacketSecondaryHeader() {
	}

public:
	/** Returns packet content as a std::vector<uint8_t> instance.
	 * @returns packet content byte array.
	 */
	std::vector<uint8_t> getAsByteVector() {
		std::vector<uint8_t> result;
		for (size_t i = 0; i < 4; i++) {
			result.push_back(time[i]);
		}
		uint8_t avalue = secondaryHeaderType.to_ulong() * 0x80 + category.to_ulong();
		result.push_back(avalue);
		result.push_back(aduCount);
		if (secondaryHeaderType.to_ulong() == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed) {
			result.push_back(aduChannelID);
			uint32_t flag_and_segmentcount = aduSegmentFlag.to_ulong() * 0x4000 + aduSegmentCount.to_ulong();
			result.push_back(flag_and_segmentcount / 0x100);
			result.push_back(flag_and_segmentcount % 0x100);
		}
		return result;
	}

public:
	/** Interprets an input byte array as Secondary Header.
	 * @param[in] data a byte array that contains CCSDS SpacePacket Secondary Header.
	 * @param[in] length of the byte array.
	 */
	void interpret(uint8_t* data, size_t length) throw (CCSDSSpacePacketException) {
		using namespace std;
		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::SecondaryHeaderTooShort);
		}
		time[0] = *data;
		time[1] = *(data + 1);
		time[2] = *(data + 2);
		time[3] = *(data + 3);
		secondaryHeaderType = bitset<1>((data[4] & 0x80) >> 7 /* 1000 0000 */);
		category = bitset<7>(data[4] & 0x3F/* 0111 1111 */);
		aduCount = data[5];
		bitset<6> aduSegmentCount_lsb8bits;
		bitset<6> aduSegmentCount_msb6bits;
		if (secondaryHeaderType.to_ulong() == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed) {
			if (length < 9) {
				throw CCSDSSpacePacketException(CCSDSSpacePacketException::SecondaryHeaderTooShort);
			}
			aduChannelID = data[6];
			aduSegmentFlag = bitset<2>((data[7] & 0xc0) >> 6 /* 1100 0000 */);

			aduSegmentCount_msb6bits = bitset<6>(data[7] & 0x3F/* 0011 1111 */);
			aduSegmentCount_lsb8bits = bitset<6>(data[8]);
			aduSegmentCount = bitset<14>();
			for (size_t i = 0; i < 6; i++) {
				aduSegmentCount.set(i, aduSegmentCount_msb6bits[i]);
			}
			for (size_t i = 0; i < 8; i++) {
				aduSegmentCount.set(i + 6, aduSegmentCount_lsb8bits[i]);
			}
		}
	}

public:
	/** Returns ADU Channel ID.
	 * @returns ADU Channel ID.
	 */
	uint8_t getADUChannelID() const {
		return aduChannelID;
	}

public:
	/** Returns ADU Count.
	 * @return ADU Count.
	 */
	uint8_t getADUCount() const {
		return aduCount;
	}

public:
	/** Returns ADU Segment Count.
	 * @returns ADU Segment Count.
	 */
	std::bitset<14> getADUSegmentCount() const {
		return aduSegmentCount;
	}

public:
	/** Returns ADU Segment Flag.
	 * @returns ADU Segment Flag.
	 * @retval 00b continuation segment
	 * @retval 01b the first segment
	 * @retval 10b the last segment
	 * @retval 11b unsegmented ADU
	 */
	std::bitset<2> getADUSegmentFlag() const {
		return aduSegmentFlag;
	}

public:
	/** Returns the Category field value.
	 * @return 7-bit Category of this packet.
	 */
	std::bitset<7> getCategory() const {
		return category;
	}

public:
	/** Returns Length of the Secondary Header part.
	 * Presence of ADU Channel is automatically checked,
	 * and reflected to a result.
	 * @returns length of the Secondary Header part.
	 */
	size_t getLength() {
		if (secondaryHeaderType == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsNotUsed) {
			return SecondaryHeaderLengthWithoutADUChannel;
		} else {
			return SecondaryHeaderLengthWithADUChannel;
		}
	}

public:
	/** Returns Secondary Header Type.
	 * @returns Secondary Header Type.
	 * @retval 0 ADU Channel is not used.
	 * @retval 1 ADU Channel is used.
	 */
	std::bitset<1> getSecondaryHeaderType() const {
		return secondaryHeaderType;
	}

public:
	/** Returns Time.
	 * @returns 32-bit Time field value.
	 */
	std::vector<uint8_t> getTime() const {
		std::vector<uint8_t> result;
		for (size_t i = 0; i < 4; i++) {
			result.push_back(time[i]);
		}
		return result;
	}

public:
	/** Sets ADU Channel ID.
	 * @param[in] aduChannelID ADU Channel ID.
	 */
	void setADUChannelID(uint8_t aduChannelID) {
		this->aduChannelID = aduChannelID;
	}

public:
	/** Sets ADU Count.
	 * @param[in] aduCount ADU Count which should be maintained channel by channel and incremented packet by packet.
	 */
	void setADUCount(uint8_t aduCount) {
		this->aduCount = aduCount;
	}

public:
	/** Sets ADU Segment Count.
	 * @param[in] aduSegmentCount ADU Segment Count.
	 */
	void setADUSegmentCount(std::bitset<14> aduSegmentCount) {
		this->aduSegmentCount = aduSegmentCount;
	}

public:
	/** Sets ADU Segment Count.
	 * @param[in] aduSegmentCount ADU Segment Count.
	 */
	void setADUSegmentCount(size_t aduSegmentCount) {
		this->aduSegmentCount = std::bitset<14>(aduSegmentCount);
	}

public:
	/** Sets ADU Segment Flag.
	 * @param[in] aduSegmentFlag ADU Segment Flag.
	 * @attention 00b continuation segment
	 * @attention 01b the first segment
	 * @attention 10b the last segment
	 * @attention 11b unsegmented ADU
	 */
	void setADUSegmentFlag(std::bitset<2> aduSegmentFlag) {
		this->aduSegmentFlag = aduSegmentFlag;
	}

public:
	/** Sets ADU Segment Flag.
	 * @param[in] aduSegmentFlag ADU Segment Flag.
	 * @attention 00b continuation segment
	 * @attention 01b the first segment
	 * @attention 10b the last segment
	 * @attention 11b unsegmented ADU
	 */
	void setADUSegmentFlag(uint32_t aduSegmentFlag) {
		this->aduSegmentFlag = std::bitset<2>(aduSegmentFlag);
	}

public:
	/** Sets the Category field.
	 * @param[in] 7-bit category field value.
	 */
	void setCategory(std::bitset<7> category) {
		this->category = category;
	}

public:
	/** Sets the Category field.
	 * @param[in] 7-bit category field value.
	 */
	void setCategory(uint8_t category) {
		this->category = std::bitset<7>(category);
	}

public:
	/** Sets Secondary Header Type.
	 * @param[in] secondaryHeaderType 0: ADU Channel is not used. 1: ADU Channel is used.
	 */
	void setSecondaryHeaderType(std::bitset<1> secondaryHeaderType) {
		this->secondaryHeaderType = secondaryHeaderType;
	}

public:
	/** Sets the Time field.
	 * @param[in] time 32-bit Time field value.
	 */
	void setTime(uint8_t* time) {
		for (size_t i = 0; i < 4; i++) {
			this->time[i] = time[i];
		}
	}

public:
	/** Sets the Time field.
	 * @param[in] time 32-bit Time field value.
	 */
	void setTime(uint32_t time) {
		this->time[0] = (time & 0xFF000000) >> 24;
		this->time[1] = (time & 0x00FF0000) >> 16;
		this->time[2] = (time & 0x0000FF00) >> 8;
		this->time[3] = (time & 0x000000FF) >> 0;
	}

public:
	/** Converts an instance to string.
	 * @returns string dump of this instance.
	 */
	std::string toString() {
		using namespace std;
		stringstream ss;

		uint64_t time_integer = 0;
		uint64_t factor = 1;
		for (int i = 0; i < 4; i++) {
			time_integer += factor * time[0];
			factor *= 0x100;
		}

		ss << "SecondaryHeader" << endl;
		ss << "Time                : " << time_integer << endl;
		ss << "SecondaryHeaderType : " << secondaryHeaderType.to_string() << endl;
		ss << "Category            : " << category.to_string() << endl;
		ss << "ADUCount            : " << (uint32_t) aduCount << endl;
		ss << "ADUChannelID        : " << (uint32_t) aduChannelID << endl;
		ss << "ADUSegmentFlag      : " << aduSegmentFlag.to_string() << endl;
		ss << "ADUSegmentCount     : " << aduSegmentCount.to_ulong() << endl;
		return ss.str();
	}

};

#endif /* CCSDSSPACEPACKETSECONDARYHEADER_HH_ */
