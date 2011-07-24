/*
 * CCSDSSpacePacket.hh
 *
 *  Created on: Jun 9, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKET_HH_
#define CCSDSSPACEPACKET_HH_

#include "CCSDSSpacePacketPrimaryHeader.hh"
#include "CCSDSSpacePacketSecondaryHeader.hh"
#include "CCSDSSpacePacketException.hh"
#include <vector>
#include <iomanip>
#include <sstream>

class CCSDSSpacePacket {
public:
	static const unsigned int APIDOfIdlePacket = 0x7FF;//111 1111 1111

public:
	CCSDSSpacePacketPrimaryHeader* primaryHeader;
	CCSDSSpacePacketSecondaryHeader* secondaryHeader;
	std::vector<unsigned char>* userDataField;

public:
	CCSDSSpacePacket() {
		primaryHeader = new CCSDSSpacePacketPrimaryHeader();
		secondaryHeader = new CCSDSSpacePacketSecondaryHeader();
		userDataField = new std::vector<unsigned char>();

		primaryHeader->setPacketVersionNum(CCSDSSpacePacketPacketVersionNumber::Version1);
	}

	~CCSDSSpacePacket() {
		delete primaryHeader;
		delete secondaryHeader;
		delete userDataField;
	}

public:
	std::vector<unsigned char> getAsByteVector() {
		calcPacketDataLength();
		std::vector<unsigned char> result;
		std::vector<unsigned char> primaryHeaderVector = primaryHeader->getAsByteVector();
		result.insert(result.end(), primaryHeaderVector.begin(), primaryHeaderVector.end());
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			std::vector<unsigned char> secondaryHeaderVector = secondaryHeader->getAsByteVector();
			result.insert(result.end(), secondaryHeaderVector.begin(), secondaryHeaderVector.end());
		}
		result.insert(result.end(), userDataField->begin(), userDataField->end());
		return result;
	}

public:
	void interpret(unsigned char* data, unsigned int length) throw (CCSDSSpacePacketException) {
		using namespace std;
		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::NotACCSDSSpacePacket);
		}
		//primary header
		primaryHeader->interpret(data);

		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::NotPresent) {
			//data field
			userDataField->clear();
			for (unsigned int i = CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength; i < length; i++) {
				userDataField->push_back(data[i]);
			}
		} else {
			//secondary header
			secondaryHeader->interpret(//
					data + CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength, //
					length - CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength);
			//data field
			userDataField->clear();
			for (unsigned int i = CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength + secondaryHeader->getLength(); i
					< length; i++) {
				userDataField->push_back(data[i]);
			}
		}
	}

	void interpret(std::vector<unsigned char>& data) {
		if (data.size() != 0) {
			interpret(&(data[0]), data.size());
		}
	}

public:
	void calcPacketDataLength() {
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			primaryHeader->setPacketDataLength(secondaryHeader->getAsByteVector().size() + userDataField->size() - 1);
		} else {
			primaryHeader->setPacketDataLength(userDataField->size() - 1);
		}
	}

	void setPacketDataLength() {
		calcPacketDataLength();
	}

	void setUserDataField(std::vector<unsigned char>* userDataField) {
		*(this->userDataField) = *userDataField;
	}

	void setUserDataField(std::vector<unsigned char> userDataField) {
		*(this->userDataField) = userDataField;
	}

public:
public:
	virtual std::string toString() {
		std::stringstream ss;
		using std::endl;
		ss << "---------------------------------" << endl;
		ss << "CCSDSSpacePacket" << endl;
		ss << "---------------------------------" << endl;
		ss << primaryHeader->toString();

		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			secondaryHeader->toString();
		} else {
			ss << "No secondary header" << endl;
		}

		if (userDataField->size() != 0) {
			ss << "User data field has " << std::dec << userDataField->size();
			if (userDataField->size() < 2) {
				ss << " byte" << endl;
			} else {
				ss << " bytes" << endl;
			}
			ss << arrayToString(userDataField, "hex") << endl;
		} else {
			ss << "No user data field" << endl;
		}
		ss << endl;
		return ss.str();
	}

	virtual void dump(std::ostream& os) {
		os << this->toString();
	}

	virtual void dump(std::ostream* os) {
		*os << this->toString();
	}

	virtual void dumpToScreen() {
		dump(std::cout);
	}

public:
	static std::string arrayToString(std::vector<unsigned char>* data, std::string mode = "dec",
			int maxBytesToBeDumped = 8) {
		//copied from CxxUtilities::Array

		using namespace std;
		stringstream ss;
		int maxSize;
		if (data->size() < maxBytesToBeDumped) {
			maxSize = data->size();
		} else {
			maxSize = maxBytesToBeDumped;
		}
		for (int i = 0; i < maxSize; i++) {
			if (mode == "dec") {
				ss << dec << left << (uint32_t) data->at(i);
			} else if (mode == "hex") {
				ss << hex << "0x" << setw(2) << setfill('0') << right << (uint32_t) data->at(i);
			} else {
				ss << data->at(i);
			}
			if (i != maxSize - 1) {
				ss << " ";
			}
		}
		ss << dec;
		if (maxSize < data->size()) {
			ss << " ... (total size = " << dec << data->size();
			if (data->size() == 1) {
				ss << " entry)";
			} else {
				ss << " entries)";
			}
		}
		return ss.str();
	}

};

#endif /* CCSDSSPACEPACKET_HH_ */
