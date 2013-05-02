/*
 * CCSDSSpacePacketPrimaryHeader.hh
 *
 *  Created on: Jun 9, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKETPRIMARYHEADER_HH_
#define CCSDSSPACEPACKETPRIMARYHEADER_HH_

#include <bitset>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

#if (defined(__GXX_EXPERIMENTAL_CXX0X) || (__cplusplus >= 201103L))
#include <cstdint>
#else
#include <stdint.h>
#endif

class CCSDSSpacePacketPacketVersionNumber {
public:
	enum {
		Version1 = 0x000, //000b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketPacketType {
public:
	enum {
		TelemetryPacket = 0x00, //0b
		CommandPacket = 0x01, //1b
		Undefined = 0xffff

	};
};

class CCSDSSpacePacketSecondaryHeaderFlag {
public:
	enum {
		NotPresent = 0x00, //0b
		Present = 0x01, //1b
		Undefined = 0xffff
	};
};

class CCSDSSpacePacketSequenceFlag {
public:
	enum {
		ContinuationSegment = 0x00, //00b
		TheFirstSegment = 0x01, //01b
		TheLastSegment = 0x02, //10b
		UnsegmentedUserData = 0x03, //11b
		Undefined = 0xffff
	};
};

/** A class that represents the Primary Header part of a CCSDS SpacePacket.
 * @see CCSDSSpacePacket for detailed usage.
 */
class CCSDSSpacePacketPrimaryHeader {
private:
	std::bitset<3> packetVersionNum;
	std::bitset<1> packetType;
	std::bitset<1> secondaryHeaderFlag;
	std::bitset<11> apid;
	std::bitset<2> sequenceFlag;
	std::bitset<14> sequenceCount;
	std::bitset<16> packetDataLength;

public:
	static const size_t PrimaryHeaderLength = 6;
	static const size_t MaximumLengthOfDataFieldOfTCPacket = 1010;
	static const size_t MaximumLengthOfDataFieldOfTMPacketWithoutADUChannel = 1012;
	static const size_t MaximumLengthOfDataFieldOfTMPacketWithADUChannel = 1009;

public:
	/** Constructor.
	 */
	CCSDSSpacePacketPrimaryHeader() {
		this->setPacketVersionNum(CCSDSSpacePacketPacketVersionNumber::Version1);
	}

public:
	/** Destructor.
	 */
	virtual ~CCSDSSpacePacketPrimaryHeader() {
	}

public:
	/** Returns packet content as a std::vector<uint8_t> instance.
	 * @returns packet content byte array.
	 */
	std::vector<uint8_t> getAsByteVector() {
		std::vector<uint8_t> result;
		std::string str = packetVersionNum.to_string() + packetType.to_string() + secondaryHeaderFlag.to_string()
				+ apid.to_string() + sequenceFlag.to_string() + sequenceCount.to_string();
		size_t length = packetDataLength.to_ulong();
		std::bitset<32> bits;
		for (size_t i = 0; i < 32; i++) {
			if (str[i] == '0') {
				bits.set(32 - i - 1, 0);
			} else {
				bits.set(32 - i - 1, 1);
			}
		}
		result.push_back((uint8_t) (bits >> 24).to_ulong());
		result.push_back((uint8_t) ((bits << 8) >> 24).to_ulong());
		result.push_back((uint8_t) ((bits << 16) >> 24).to_ulong());
		result.push_back((uint8_t) ((bits << 24) >> 24).to_ulong());
		result.push_back((uint8_t) (length / 0x100));
		result.push_back((uint8_t) (length % 0x100));
		return result;
	}

public:
	/** Interprets an input byte array as Primary Header.
	 * @param[in] data a byte array that contains CCSDS SpacePacket Primary Header.
	 */
	void interpret(const uint8_t* data) {
		using namespace std;
		packetVersionNum = bitset<3>((data[0] & 0xe0) >> 5 /* 1110 0000 */);
		packetType = bitset<1>((data[0] & 0x10) >> 4 /* 0001 0000 */);
		secondaryHeaderFlag = bitset<1>((data[0] & 0x08) >> 3 /* 0000 1000 */);
		bitset<3> apid_msb3bits(data[0] & 0x07);
		bitset<8> apid_lsb8bits(data[1]);
		for (size_t i = 0; i < 3; i++) {
			apid.set(i + 8, apid_msb3bits[i]);
		}
		for (size_t i = 0; i < 8; i++) {
			apid.set(i, apid_lsb8bits[i]);
		}
		/*
		 cout << "#data[1]=" << (uint32_t) data[1] << endl;
		 cout << "#apid_lsb8bits=" << apid_lsb8bits.to_string() << endl;
		 cout << "#apid         =" << apid.to_string() << endl;
		 cout << "#apid=" << (uint32_t) apid.to_ulong() << endl;
		 */
		sequenceFlag = bitset<2>((data[2] & 0xc0) >> 6 /* 1100 0000 */);
		bitset<6> sequenceCount_msb6bits(data[2] & 0x3F/* 0011 1111 */);
		bitset<6> sequenceCount_lsb8bits(data[3]);
		for (size_t i = 0; i < 6; i++) {
			sequenceCount.set(i + 8, sequenceCount_msb6bits[i]);
		}
		for (size_t i = 0; i < 8; i++) {
			sequenceCount.set(i, sequenceCount_lsb8bits[i]);
		}
		packetDataLength = bitset<16>(data[4] * 0x100 + data[5]);
	}

public:
	/** Returns APID as std::bitset<11>. */
	inline std::bitset<11> getAPID() const {
		return apid;
	}

public:
	/** Returns APDI as an integer. */
	inline size_t getAPIDAsInteger() const {
		return apid.to_ulong();
	}

public:
	/** Returns Packet Data Length.
	 * @returns (Total number of bytes in the Packet Data field - 1).
	 */
	inline size_t getPacketDataLength() const {
		return packetDataLength.to_ulong();
	}

public:
	/** Returns Packet Type.
	 * @retval 0 Command packet.
	 * @retval 1 Telemetry Packet.
	 */
	inline std::bitset<1> getPacketType() const {
		return packetType;
	}

public:
	/** Returns Packet Version Number.
	 * @retval 000 Version 1.
	 */
	inline std::bitset<3> getPacketVersionNum() const {
		return packetVersionNum;
	}

public:
	/** Returns Secondary Header Flag.
	 * @returns Secondary Header Flag.
	 * @retval 1 Secondary Header is present.
	 * @retval 0 Secondary Header is not present.
	 */
	inline std::bitset<1> getSecondaryHeaderFlag() const {
		return secondaryHeaderFlag;
	}

public:
	/** Returns Packet Sequence Count. */
	inline std::bitset<14> getSequenceCount() const {
		return sequenceCount;
	}

public:
	/** Returns Packet Sequence Flag.
	 * @retval 00 Continuation segment of user data.
	 * @retval 01 First segment of user data.
	 * @retval 10 Last segment of user data.
	 * @retval 11 Unsegmented user data.
	 */
	inline std::bitset<2> getSequenceFlag() const {
		return sequenceFlag;
	}

public:
	/** True if Packet is segmented.
	 */
	inline bool isSegmented() {
		return (sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::TheFirstSegment
				|| sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::TheLastSegment
				|| sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::ContinuationSegment) ? true : false;
	}

public:
	/** True if Packet is the first segmented.
	 */
	inline bool isFirstSegment(){
		return (sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::TheFirstSegment) ? true : false;
	}

public:
	/** True if Packet is the last segmented.
	 */
	inline bool isLastSegment(){
		return (sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::TheLastSegment) ? true : false;
	}

public:
	/** True if Packet is a continuation segmented.
	 */
	inline bool isContinuationSegment(){
		return (sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::ContinuationSegment) ? true : false;
	}

public:
	/** True if Packet is an unsegmented packet.
	 */
	inline bool isUnsegmented(){
		return (sequenceFlag.to_ulong() == CCSDSSpacePacketSequenceFlag::UnsegmentedUserData) ? true : false;
	}

public:
	/** Sets APID from a uint16_t integer.
	 * @param[in] apid APID.
	 */
	inline void setAPID(uint16_t apid) {
		this->apid = std::bitset<11>(apid);
	}

public:
	/** Sets Packet Data Length field.
	 * @param packetDataLength Packet Data Length value.
	 */
	inline void setPacketDataLength(std::bitset<16> packetDataLength) {
		this->packetDataLength = packetDataLength;
	}

public:
	/** Sets Packet Data Length field.
	 * @param packetDataLength Packet Data Length value.
	 */
	inline void setPacketDataLength(size_t packetDataLength) {
		this->packetDataLength = std::bitset<16>(packetDataLength);
	}

public:
	/** Sets Packet Type.
	 * @param[in] packetType.
	 * @attention packetType==0:Command packet.
	 * @attention packetType==1:Telemetry Packet.
	 */
	inline void setPacketType(uint32_t packetType) {
		this->packetType = std::bitset<1>(packetType);
	}

private:
	/** Sets Packet Version Number.
	 * @param[in] packetVersionNum 000 for Version 1.
	 */
	inline void setPacketVersionNum(std::bitset<3> packetVersionNum) {
		this->packetVersionNum = packetVersionNum;
	}

public:
	/** Sets Packet Version Number.
	 * @param[in] packetVersionNum 000 for Version 1.
	 */
	inline void setPacketVersionNum(uint32_t packetVersionNum) {
		this->packetVersionNum = std::bitset<3>(packetVersionNum);
	}

public:
	/** Sets Secondary Header Flag.
	 * @param[in] secondaryHeaderFlag Secondary Header Flag.
	 * @attention secondaryHeaderFlag==0: Secondary Header is not present.
	 * @attention secondaryHeaderFlag==1: Secondary Header is present.
	 */
	inline void setSecondaryHeaderFlag(std::bitset<1> secondaryHeaderFlag) {
		this->secondaryHeaderFlag = secondaryHeaderFlag;
	}

public:
	/** Sets Secondary Header Flag.
	 * @param[in] secondaryHeaderFlag Secondary Header Flag.
	 * @attention secondaryHeaderFlag==0: Secondary Header is not present.
	 * @attention secondaryHeaderFlag==1: Secondary Header is present.
	 */
	inline void setSecondaryHeaderFlag(uint8_t secondaryHeaderFlag) {
		this->secondaryHeaderFlag = std::bitset<1>(secondaryHeaderFlag);
	}

public:
	/** Sets Packet Sequence Count.
	 * @paarm[in] sequenceCount Packet Sequence Count.
	 */
	inline void setSequenceCount(std::bitset<14> sequenceCount) {
		this->sequenceCount = sequenceCount;
	}

public:
	/** Sets Packet Sequence Count.
	 * @paarm[in] sequenceCount Packet Sequence Count.
	 */
	inline void setSequenceCount(size_t sequenceCount) {
		this->sequenceCount = std::bitset<14>(sequenceCount);
	}

public:
	/** Sets Packet Sequence Flag.
	 * @paarm[in] sequenceFlag Packet Sequence Flag.
	 */
	inline void setSequenceFlag(std::bitset<2> sequenceFlag) {
		this->sequenceFlag = sequenceFlag;
	}

public:
	/** Sets Packet Sequence Flag.
	 * @paarm[in] sequenceFlag Packet Sequence Flag.
	 */
	inline void setSequenceFlag(uint32_t sequeceFlag) {
		this->sequenceFlag = std::bitset<2>(sequeceFlag);
	}

public:
	/** Converts an instance to string.
	 * @returns string dump of this instance.
	 */
	virtual std::string toString() {
		using namespace std;
		std::stringstream ss;
		ss << "PrimaryHeader" << endl;
		ss << "PacketVersionNum    : " << packetVersionNum.to_string() << endl;
		ss << "PacketType          : " << packetType.to_string() << endl;
		ss << "SecondaryHeaderFlag : " << secondaryHeaderFlag.to_string() << endl;
		ss << "APID                : " << apid.to_ulong();
		ss << " (0x" << hex << setw(2) << setfill('0') << right << apid.to_ulong() << ")" << left << endl;
		ss << "SequenceFlag        : " << sequenceFlag.to_string();
		switch (sequenceFlag.to_ulong()) {
		case 0:
			ss << " (Continuation segment of user data)" << endl;
			break;
		case 1:
			ss << " (First segment of user data)" << endl;
			break;
		case 2:
			ss << " (Last segment of user data)" << endl;
			break;
		case 3:
			ss << " (Unsegmented user data)" << endl;
			break;
		}
		ss << "SequenceCount       : " << sequenceCount.to_ulong() << endl;
		ss << "PacketDataLength    : " << dec << packetDataLength.to_ulong() << " (0x" << hex << right << setw(4) << setfill('0')  << packetDataLength.to_ulong()<<")" ;
		ss << " (Packet Data Field has " << dec << packetDataLength.to_ulong() + 1 << " bytes)" << endl;
		return ss.str();
	}
};

#endif /* CCSDSSPACEPACKETPRIMARYHEADER_HH_ */
