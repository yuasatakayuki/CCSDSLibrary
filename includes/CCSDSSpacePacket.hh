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

class CCSDSSpacePacket {
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
	void interpretAsCommandMessage(unsigned char* data, unsigned int length) throw (CCSDSSpacePacketException) {
		using namespace std;
		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::NotACCSDSSpacePacket);
		}
		//primary header
		primaryHeader->interpret(data);

		if(primaryHeader->getSecondaryHeaderFlag().to_ulong()==CCSDSSpacePacketSecondaryHeaderFlag::NotPresent){
			//data field
			userDataField->clear();
			for(unsigned int i=CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength;i<length;i++){
				userDataField->push_back(data[i]);
			}
		}else{
			//secondary header
			secondaryHeader->interpret(data[CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength],length-CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength);
			//data field
			userDataField->clear();
			for(unsigned int i=CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength+secondaryHeader->getLength();i<length;i++){
				userDataField->push_back(data[i]);
			}
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

	void setUserDataField(std::vector<unsigned char>* userDataField) {
		*(this->userDataField) = *userDataField;
	}

	void setUserDataField(std::vector<unsigned char> userDataField) {
		*(this->userDataField) = userDataField;
	}

};

#endif /* CCSDSSPACEPACKET_HH_ */
