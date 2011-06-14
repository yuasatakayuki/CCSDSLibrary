/*
 * CCSDSSpacePacketException.hh
 *
 *  Created on: Jun 14, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKETEXCEPTION_HH_
#define CCSDSSPACEPACKETEXCEPTION_HH_

class CCSDSSpacePacketException {
public:
	enum {
		NotACCSDSSpacePacket = 0x01, SecondaryHeaderTooShort = 0x10,

	};
public:
	unsigned int status;

public:
	unsigned int getStatus() const {
		return status;
	}

	void setStatus(unsigned int status) {
		this->status = status;
	}

public:
	CCSDSSpacePacketException(unsigned int status) {
		this->status = status;
	}

};
#endif /* CCSDSSPACEPACKETEXCEPTION_HH_ */
