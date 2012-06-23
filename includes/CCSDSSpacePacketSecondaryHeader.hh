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
	unsigned char time[4];
	std::bitset<1> secondaryHeaderType;
	std::bitset<7> category;
	unsigned char aduCount;
	unsigned char aduChannelID;
	std::bitset<2> aduSegmentFlag;
	std::bitset<14> aduSegmentCount;

public:
	static const unsigned int SecondaryHeaderLengthWithoutADUChannel = 6;
	static const unsigned int SecondaryHeaderLengthWithADUChannel = 8;

public:
	std::vector<unsigned char> getAsByteVector() {
		std::vector<unsigned char> result;
		for (unsigned int i = 0; i < 4; i++) {
			result.push_back(time[i]);
		}
		unsigned char avalue = secondaryHeaderType.to_ulong() * 0x80 + category.to_ulong();
		result.push_back(avalue);
		result.push_back(aduCount);
		if (secondaryHeaderType.to_ulong() == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed) {
			result.push_back(aduChannelID);
			unsigned int flag_and_segmentcount = aduSegmentFlag.to_ulong() * 0x4000 + aduSegmentCount.to_ulong();
			result.push_back(flag_and_segmentcount / 0x100);
			result.push_back(flag_and_segmentcount % 0x100);
		}
		return result;
	}

public:
	void interpret(unsigned char* data, unsigned int length) throw (CCSDSSpacePacketException) {
		using namespace std;
		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::SecondaryHeaderTooShort);
		}
		time[0] = *data;
		time[1] = *(data + 1);
		time[2] = *(data + 2);
		time[3] = *(data + 3);
		secondaryHeaderType = bitset<1> ((data[4] & 0x80) >> 7 /* 1000 0000 */);
		category = bitset<7> (data[4] & 0x3F/* 0111 1111 */);
		aduCount = data[5];
		bitset<6> aduSegmentCount_lsb6bits;
		bitset<6> aduSegmentCount_msb6bits;
		if (secondaryHeaderType.to_ulong() == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed) {
			if (length < 9) {
				throw CCSDSSpacePacketException(CCSDSSpacePacketException::SecondaryHeaderTooShort);
			}
			aduChannelID = data[6];
			aduSegmentFlag = bitset<2> ((data[7] & 0xc0) >> 6 /* 1100 0000 */);

			aduSegmentCount_msb6bits = bitset<6> (data[7] & 0x3F/* 0011 1111 */);
			aduSegmentCount_lsb6bits = bitset<6> (data[8]);
			aduSegmentCount = bitset<14> ();
		}
		for (unsigned int i = 0; i < 6; i++) {
			aduSegmentCount.set(i, aduSegmentCount_msb6bits[i]);
		}
		for (unsigned int i = 0; i < 8; i++) {
			aduSegmentCount.set(i + 6, aduSegmentCount_lsb6bits[i]);
		}
	}

public:
	unsigned char getADUChannelID() const {
		return aduChannelID;
	}

	unsigned char getADUCount() const {
		return aduCount;
	}

	std::bitset<14> getADUSegmentCount() const {
		return aduSegmentCount;
	}

	std::bitset<2> getADUSegmentFlag() const {
		return aduSegmentFlag;
	}

	std::bitset<7> getCategory() const {
		return category;
	}

	unsigned int getLength() {
		if (secondaryHeaderType == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsNotUsed) {
			return SecondaryHeaderLengthWithoutADUChannel;
		} else {
			return SecondaryHeaderLengthWithADUChannel;
		}
	}

	std::bitset<1> getSecondaryHeaderType() const {
		return secondaryHeaderType;
	}

	std::vector<unsigned char> getTime() const {
		std::vector<unsigned char> result;
		for (unsigned int i = 0; i < 4; i++) {
			result.push_back(time[i]);
		}
		return result;
	}

	void setADUChannelID(unsigned char aduChannelID) {
		this->aduChannelID = aduChannelID;
	}

	void setADUCount(unsigned char aduCount) {
		this->aduCount = aduCount;
	}

	void setADUSegmentCount(std::bitset<14> aduSegmentCount) {
		this->aduSegmentCount = aduSegmentCount;
	}

	void setADUSegmentCount(unsigned int aduSegmentCount) {
		this->aduSegmentCount = std::bitset<14>(aduSegmentCount);
	}

	void setADUSegmentFlag(std::bitset<2> aduSegmentFlag) {
		this->aduSegmentFlag = aduSegmentFlag;
	}

	void setADUSegmentFlag(unsigned int aduSegmentFlag) {
		this->aduSegmentFlag = std::bitset<2>(aduSegmentFlag);
	}

	void setCategory(std::bitset<7> category) {
		this->category = category;
	}

	void setCategory(unsigned int category) {
		this->category = std::bitset<7>(category);
	}

	void setSecondaryHeaderType(std::bitset<1> secondaryHeaderType) {
		this->secondaryHeaderType = secondaryHeaderType;
	}

	void setTime(unsigned char* time) {
		for (unsigned int i = 0; i < 4; i++) {
			this->time[i] = time[i];
		}
	}

	void setTime(uint32_t time){
		this->time[0] = (time & 0xFF000000) >> 24;
		this->time[1] = (time & 0x00FF0000) >> 16;
		this->time[2] = (time & 0x0000FF00) >> 8;
		this->time[3] = (time & 0x000000FF) >> 0;
	}

public:
	std::string toString() {
		using namespace std;
		stringstream ss;

		uint64_t time_integer=0;
		uint64_t factor=1;
		for(int i=0;i<4;i++){
			time_integer+=factor*time[0];
			factor*=0x100;
		}

		ss << "SecondaryHeader" << endl;
		ss << "Time                : " << time_integer << endl;
		ss << "SecondaryHeaderType : " << secondaryHeaderType.to_string() << endl;
		ss << "Category            : " << category.to_string() << endl;
		ss << "ADUCount            : " << (uint32_t)aduCount << endl;
		ss << "ADUChannelID        : " << (uint32_t)aduChannelID << endl;
		ss << "ADUSegmentFlag      : " << aduSegmentFlag.to_string() << endl;
		ss << "ADUSegmentCount     : " << aduSegmentCount.to_ulong() << endl;
		return ss.str();
	}

};

#endif /* CCSDSSPACEPACKETSECONDARYHEADER_HH_ */
