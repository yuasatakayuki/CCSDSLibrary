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
	static const unsigned int SecondaryHeaderLengthWithoutADUChannel = 6;
	static const unsigned int SecondaryHeaderLengthWithADUChannel = 8;

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
	void interpret(unsigned char* data, unsigned int length) throw (CCSDSSpacePacketException) {
		using namespace std;
		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::SecondaryHeaderTooShort);
		}
		time[0] = data[0];
		time[1] = data[1];
		time[2] = data[2];
		time[3] = data[3];
		secondaryHeaderType = bitset<1> ((data[4] & 0x80) >> 7 /* 1000 0000 */);
		category = bitset<7> (data[4] & 0x3F/* 0111 1111 */);
		aduCount = data[5];
		if (secondaryHeaderType.to_ulong() == CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed) {
			if (length < 9) {
				throw CCSDSSpacePacketException(CCSDSSpacePacketException::SecondaryHeaderTooShort);
			}
			aduChannelID = data[6];
			aduSegmentFlag = bitset<2> ((data[7] & 0xc0) >> 6 /* 1100 0000 */);

			bitset<6> aduSegmentCount_msb6bits(data[7]&0x3F/* 0011 1111 */);
			bitset<6> aduSegmentCount_lsb6bits(data[8]);
			aduSegmentCount=bitset<14>();
		}
		for(unsigned int i=0;i<6;i++){
			aduSegmentCount.set(i,aduSegmentCount_msb6bits[i]);
		}
		for(unsigned int i=0;i<8;i++){
			aduSegmentCount.set(i+6,aduSegmentCount_lsb6bits[i]);
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

};

#endif /* CCSDSSPACEPACKETSECONDARYHEADER_HH_ */
