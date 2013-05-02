/*
 * CCSDSSpacePacketException.hh
 *
 *  Created on: Jun 14, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKETEXCEPTION_HH_
#define CCSDSSPACEPACKETEXCEPTION_HH_

#if (defined(__GXX_EXPERIMENTAL_CXX0X) || (__cplusplus >= 201103L))
#include <cstdint>
#else
#include <stdint.h>
#endif

/** An exception class used by the CCSDSSpacePacket class.
 */
class CCSDSSpacePacketException {
public:
	enum {
		NotACCSDSSpacePacket = 0x01, //
		SecondaryHeaderTooShort = 0x10,
		InconsistentPacketLength
	};
public:
	uint32_t status;

public:
	/** Returns exception status.
	 * @returns exception status.
	 */
	uint32_t getStatus() const {
		return status;
	}

public:
	/** Sets exception status.
	 * @param[in] status exception status.
	 */
	void setStatus(uint32_t status) {
		this->status = status;
	}

public:
	/** Constructs an instance with an exception status.
	 * @param[in] status exception status.
	 */
	CCSDSSpacePacketException(uint32_t status) {
		this->status = status;
	}

};
#endif /* CCSDSSPACEPACKETEXCEPTION_HH_ */
