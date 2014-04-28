/*
 * ADUUnsegmenter.hpp
 *
 *  Created on: 2013/01/31
 *      Author: sakurai
 */

#ifndef ADUUNSEGMENTER_HH_
#define ADUUNSEGMENTER_HH_

#include <iostream>
#include <sstream>
#include <map>
#include <queue>

#include "CCSDSLibrary/CCSDS.hh"

/** A class that represents a complete ADU.
 */
class ADU {
public:
	ADU() {
	}

public:
	uint16_t packettype = 0x00;
	uint16_t upperAPID = 0x00;
	uint16_t lowerAPID = 0x00;
	uint16_t ADUChannelID = 0x00;
	uint8_t ADUCount = 0x00;
	uint32_t TI = 0x00000000;
	uint8_t category = 0x00;

public:
	std::vector<uint8_t> data;

public:
	bool isTMPacket() const {
		return packettype == 0;
	}

public:
	bool isTCPacket() const {
		return packettype == 1;
	}
};

class ADUSegmentsException {
public:
	ADUSegmentsException(std::string str = "") {
		message = str;
	}

public:
	std::string toString() {
		return message;
	}

public:
	std::string message;
};

class ADUUnsegmenterException {
public:
	ADUUnsegmenterException(std::string str) {
		message = str;
	}

public:
	std::string toString() {
		return message;
	}

public:
	std::string message;
};

/** A class that represents a collection of pending ADU segments (of a certain ADU Channel ID).
 */
class ADUSegments {
public:
	enum {
		ErrorSegment = -2, EmptySegment = -1
	};

public:
	uint16_t aduChannelID;
	bool watchADUSegmentCount;

private:
	bool complete;
	std::vector<CCSDSSpacePacket*> pendingPackets;
	uint16_t currentSegmentCount;
	int currentSegmentFlag;

private:
	void initialize() {
		clearAndDeletePendingPackets();
		this->currentSegmentCount = 0;
		this->currentSegmentFlag = EmptySegment;
		this->complete = false;
	}

private:
	void clearAndDeletePendingPackets() {
		for (size_t i = 0; i < pendingPackets.size(); i++) {
			delete pendingPackets[i];
		}
		pendingPackets.clear();
	}

public:
	/** Constructs an instance with a specified ADU Channel ID.
	 * @param[in] aduChannelID ADU Channel ID that this insatnce should take care of
	 */
	ADUSegments(uint16_t aduChannelID) {
		this->aduChannelID = aduChannelID;
		this->watchADUSegmentCount = true;
		initialize();
	}

public:
	/** Returns if the pending ADU segments form a complete ADU.
	 * @return true if the pending ADU segments form a complete ADU
	 */
	bool isComplete() {
		return complete;
	}

public:
	/** Join the pending ADU segments and produces a complete ADU.
	 *  The complete ADU will be stored as a newly created instance of ADU.
	 *  Deletion of the instance must be done in user application.
	 *  The pending ADU segments (their instances) will be deleted inside this method.
	 */
	ADU* unite() {
		if (!complete) {
			throw ADUSegmentsException();
		}
		ADU* adu = new ADU;
		adu->category = pendingPackets[0]->getSecondaryHeader()->getCategory().to_ulong();
		adu->ADUChannelID = this->aduChannelID;
		adu->packettype = pendingPackets[0]->getPrimaryHeader()->getPacketType().to_ulong();
		adu->upperAPID = pendingPackets[0]->getPrimaryHeader()->getAPIDAsInteger() >> 8;
		adu->lowerAPID = pendingPackets[0]->getPrimaryHeader()->getAPIDAsInteger() & 0xff;
		adu->ADUCount = pendingPackets[0]->getSecondaryHeader()->getADUCount();
		adu->TI = pendingPackets[0]->getSecondaryHeader()->getTimeAsInteger();
		for (size_t i = 0; i < pendingPackets.size(); i++) {
			std::vector<uint8_t>* nextdata = pendingPackets[i]->getUserDataField();
			adu->data.insert(adu->data.end(), nextdata->begin(), nextdata->end());
		}
		initialize();
		return adu;
	}

private:
	void error_process(std::string message = "") {
		initialize();
		currentSegmentFlag = ErrorSegment;
		throw ADUSegmentsException(message);
	}

public:
	static const size_t NMaximumADUSegmentCount = 16384;

public:
	void push(CCSDSSpacePacket* packet) {
		CCSDSSpacePacketSecondaryHeader* secondaryHeader = packet->getSecondaryHeader();
		if (complete) {
			throw ADUSegmentsException();
		} else if (secondaryHeader->getADUChannelID() != this->aduChannelID) {
			throw ADUSegmentsException();
		}

		uint16_t newSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
		std::stringstream ss;
		ss << (int) secondaryHeader->getADUChannelID() << " " << secondaryHeader->getADUSegmentFlag().to_ulong() << " "
				<< currentSegmentCount << " " << newSegmentCount;
		if (watchADUSegmentCount) {
			if (currentSegmentFlag != EmptySegment) {
				if (newSegmentCount != currentSegmentCount + 1 //i.e. newSegmentCount is different from currentSegmentCount by more than 1
				&& ((size_t) newSegmentCount + NMaximumADUSegmentCount) != ((size_t) currentSegmentCount + 1) //
						) {
					std::stringstream ss;
					ss << "!!!!!!!adu segment count jumped: " << currentSegmentCount << "-->" << newSegmentCount;
					error_process(ss.str());
				}
			}
		}

		if (secondaryHeader->isADUUnsegmented()) {
			if (currentSegmentFlag == EmptySegment || currentSegmentFlag == ErrorSegment) {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				clearAndDeletePendingPackets();
				pendingPackets.push_back(packet);
				complete = true;
			} else {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				error_process("uns");
			}
		} else if (secondaryHeader->isADUFirstSegment()) {
			if (currentSegmentFlag == EmptySegment || currentSegmentFlag == ErrorSegment) {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				clearAndDeletePendingPackets();
				pendingPackets.push_back(packet);
				complete = false;
			} else {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				error_process("1st");
			}
		} else if (secondaryHeader->isADUContinuationSegment()) {
			if (currentSegmentFlag == CCSDSSpacePacketSequenceFlag::TheFirstSegment
					|| currentSegmentFlag == CCSDSSpacePacketSequenceFlag::ContinuationSegment) {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				pendingPackets.push_back(packet);
				complete = false;
			} else {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				error_process("tbc");
			}
		} else if (secondaryHeader->isADULastSegment()) {
			if (currentSegmentFlag == CCSDSSpacePacketSequenceFlag::TheFirstSegment
					|| currentSegmentFlag == CCSDSSpacePacketSequenceFlag::ContinuationSegment) {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				pendingPackets.push_back(packet);
				complete = true;
			} else {
				currentSegmentFlag = secondaryHeader->getADUSegmentFlag().to_ulong();
				currentSegmentCount = secondaryHeader->getADUSegmentCount().to_ulong();
				error_process("fin");
			}
		} else {
			error_process("hatena");
		}
	}
};

/** A class that restores a complete ADU from ADU segments split into multiple CCSDS SpacePackets.
 * Typical usage: <br/>
 * <ol>
 * <li> Instantiate ADUUnsegmenter
 * This class was taken from HXI/SGD DataReceiver by Soki Sakurai and Hirokazu Odaka.
 */
class ADUUnsegmenter {
private:
	class ADUSegmentMap: public std::map<uint16_t, ADUSegments*> {
	};

private:
	ADUSegmentMap aduSegmentMap;
	std::queue<ADU*> completedADUs;

public:
	uint16_t lowerAPID;

public:
	ADUUnsegmenter(uint16_t lowerAPID) {
		this->lowerAPID = lowerAPID;
	}

public:
	/** Puses a CCSDS SpacePacket to the unsegmenter.
	 * Pused packet will be analyzed, and then connected to
	 * a pending packet list if it is a middle segment.
	 * If the segment is the last segment of a complete ADU,
	 * ADUUnsegmenter will join segments in the pending list,
	 * and puses a unsegmented complete ADU to an internal
	 * buffer so that user application can retrieve the complete
	 * ADU via the popCompleteADU() method.
	 * @param[in] ccsdsSpacePacketByteArray a CCSDS SpacePacket that contains an ADU segment
	 */
	void push(const std::vector<uint8_t>& ccsdsSpacePacketByteArray) throw (ADUUnsegmenterException) {
		CCSDSSpacePacket* packet = new CCSDSSpacePacket;
		packet->interpret(&ccsdsSpacePacketByteArray[0], ccsdsSpacePacketByteArray.size());
		CCSDSSpacePacketPrimaryHeader* primaryHeader = packet->getPrimaryHeader();
		CCSDSSpacePacketSecondaryHeader* secondaryHeader = packet->getSecondaryHeader();
		uint16_t lowerAPID = primaryHeader->getAPIDAsInteger() & 0xff;
		if (lowerAPID != this->lowerAPID) {
			delete packet;
			ADUUnsegmenterException e("APIDMismatch");
			throw e;
		}

		uint16_t aduChannelID = secondaryHeader->getADUChannelID();
		if (aduSegmentMap.find(aduChannelID) == aduSegmentMap.end()) {
			aduSegmentMap[aduChannelID] = new ADUSegments(aduChannelID);
			aduSegmentMap[aduChannelID]->watchADUSegmentCount = false;
		}
		ADUSegments* segments = aduSegmentMap[aduChannelID];
		try {
			segments->push(packet);
		} catch (ADUSegmentsException& e) {
			std::cerr << e.toString() << std::endl;
			throw ADUUnsegmenterException(e.toString());
		}

		if (segments->isComplete()) {
			ADU* adu = segments->unite();
			completedADUs.push(adu);
		}
	}

public:
	/** Puses a CCSDS SpacePacket to the unsegmenter.
	 * Pused packet will be analyzed, and then connected to
	 * a pending packet list if it is a middle segment.
	 * If the segment is the last segment of a complete ADU,
	 * ADUUnsegmenter will join segments in the pending list,
	 * and puses a unsegmented complete ADU to an internal
	 * buffer so that user application can retrieve the complete
	 * ADU via the popCompleteADU() method.
	 * Note: This method makes a copy of the input CCSDSSpacePacket instance,
	 * and pushes to a vector for unsegmentation. Deletion of the input
	 * CCSDSSpacePacket instance should be taken care outside this method.
	 * @param[in] ccsdsSpacePacketByteArray a CCSDS SpacePacket that contains an ADU segment
	 */
	void push(CCSDSSpacePacket* ccsdsSpacePacket) throw (ADUUnsegmenterException) {
		CCSDSSpacePacket* packet = ccsdsSpacePacket->clone(); //copy the instance
		CCSDSSpacePacketPrimaryHeader* primaryHeader = packet->getPrimaryHeader();
		CCSDSSpacePacketSecondaryHeader* secondaryHeader = packet->getSecondaryHeader();
		uint16_t lowerAPID = primaryHeader->getAPIDAsInteger() & 0xff;
		if (lowerAPID != this->lowerAPID) {
			delete packet;
			ADUUnsegmenterException e("APIDMismatch");
			throw e;
		}

		uint16_t aduChannelID = secondaryHeader->getADUChannelID();
		if (aduSegmentMap.find(aduChannelID) == aduSegmentMap.end()) {
			aduSegmentMap[aduChannelID] = new ADUSegments(aduChannelID);
			aduSegmentMap[aduChannelID]->watchADUSegmentCount = false;
		}
		ADUSegments* segments = aduSegmentMap[aduChannelID];
		try {
			segments->push(packet);
		} catch (ADUSegmentsException& e) {
			std::cerr << e.toString() << std::endl;
			throw ADUUnsegmenterException(e.toString());
		}

		if (segments->isComplete()) {
			ADU* adu = segments->unite();
			completedADUs.push(adu);
		}
	}

public:
	/** Return true if there is a complete ADU in the internal buffer.
	 */
	bool hasCompleteADU() {
		return !completedADUs.empty();
	}

public:
	/** Returns a complete ADU produced by joining ADU segments.
	 * Deletion of a pointer must be taken care in user application.
	 * If this method is invoked when there is no complete ADU in the
	 * internal buffer, nullptr will be returned.
	 * @return a pointer to a complete ADU instance
	 */
	ADU* popCompletedADU() throw (ADUUnsegmenterException) {
		if (!completedADUs.empty()) {
			ADU* product = completedADUs.front();
			completedADUs.pop();
			return product;
		} else {
			throw ADUUnsegmenterException("NoCompleteADU");
		}
	}

};

#endif /* ADUUNSEGMENTER_HH_ */
