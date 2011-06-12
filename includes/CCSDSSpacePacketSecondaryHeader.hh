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

class CCSDSSpacePacketSecondaryHeaderType {
public:
	enum {
		ADUChannelIsNotUsed = 0x00, //0b
		ADUChannelIsUsed = 0x01, //1b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketADUChannelID {
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
	std::vector<unsigned char> getAsByteVector() {
		std::vector<unsigned char> result;
		for (unsigned int i = 0; i < 4; i++) {
			result.push_back(time[i]);
		}
		unsigned char avalue = secondaryHeaderType.to_ulong() * 0x10 + category.to_ulong();
		result.push_back(avalue);
		result.push_back(aduCount);
		if (secondaryHeaderType.to_ulong() == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed) {
			result.push_back(aduChannelID);
			unsigned int flag_and_segmentcount = aduSegmentFlag.to_ulong() * 0x40 + aduSegmentCount.to_ulong();
			result.push_back(flag_and_segmentcount / 0x100);
			result.push_back(flag_and_segmentcount % 0x100);
		}
		return result;
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

};

#endif /* CCSDSSPACEPACKETSECONDARYHEADER_HH_ */
