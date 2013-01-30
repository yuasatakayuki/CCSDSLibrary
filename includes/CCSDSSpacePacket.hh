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

/** @mainpage CCSDS SpacePacket Library
 * @section intro Introduction
 * CCSDS SpacePacket Library is a header-only C++ class library
 * that manipulates packets in the CCSDS SpacePacket format.
 * Secondary Header Part is implemented for JAXA/ISAS missions.
 *
 * Related libraries:
 * - <a href="https://github.com/yuasatakayuki/SpaceWireRMAPLibrary">SpaceWire RMAP Library</a>
 * - <a href="https://github.com/yuasatakayuki/SMCPLibrary">SMCP Library</a>
 *
 * @section install Installation and Build
 * Git repository is hosted on the github.com.
 * To install the latest source tree, execute git clone like below.
 * @code
 * git clone https://github.com/yuasatakayuki/CCSDSLibrary.git
 * @endcode
 *
 * The library does not need to be pre-compiled since this consists
 * only from header files. Just include CCSDSLibrary/includes/CCSDS.hh
 * and provide a header search path to your compiler.
 *
 * For example, in a source file,
 * @code
 * #include "CCSDS.hh"
 * ...your source...
 * @endcode
 * and compile it with a -I flag,
 * @code
 * g++ -I/path/to/CCSDSLibrary/includes your_application.cc -o your_application
 * @endcode
 *
 * @section classes Classes
 * Users mainly interface with instances of the CCSDSSpacePacket class.
 * CCSDSSpacePacket internally contains
 * - Primary Header (the CCSDSSpacePacketPrimaryHeader class)
 * - Secondary Header (if used; the CCSDSSpacePacketSecondaryHeader class)
 * - User Data Field (std::vector<uint8_t>)
 *
 * See <a href="annotated.html">Class List</a> for complete API reference.
 *
 * @section usage Example Usages
 * @subsection example_creation Packet creation
 * @code
	//constructs an empty instance
	CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();

	//set APID
	ccsdsPacket->getPrimaryHeader()->setAPID(apid);

	//set Packet Type (Telemetry or Command)
	ccsdsPacket->getPrimaryHeader()->setPacketType(CCSDSSpacePacketPacketType::TelemetryPacket);

	//set Secondary Header Flag (whether this packet has the Secondary Header part)
	ccsdsPacket->getPrimaryHeader()->setSecondaryHeaderFlag(CCSDSSpacePacketSecondaryHeaderFlag::Present);

	//set segmentation information
	ccsdsPacket->getPrimaryHeader()->setSequenceFlag(CCSDSSpacePacketSequenceFlag::UnsegmentedUserData);

	//set Category
	ccsdsPacket->getSecondaryHeader()->setCategory(category);

	//set secondary header type (whether ADU Channel presence)
	ccsdsPacket->getSecondaryHeader()->
	   setSecondaryHeaderType(CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed);

	//set ADU Channel ID
	ccsdsPacket->getSecondaryHeader()->setADUChannelID(0x00);

	//set ADU Segmentation Flag (whether ADU is segmented)
	ccsdsPacket->getSecondaryHeader()->setADUSegmentFlag(CCSDSSpacePacketADUSegmentFlag::UnsegmentedADU);

	//set counters
	ccsdsPacket->getPrimaryHeader()->setSequenceCount(sequenceCount);
	ccsdsPacket->getSecondaryHeader()->setADUCount(aduCount);

	//set absolute time
	uint8_t time[4];
	ccsdsPacket->getSecondaryHeader()->setTime(time);

	//set data
	ccsdsPacket->setUserDataField(smcpByteArray);
	ccsdsPacket->setPacketDataLength();

	//get packet as byte array
	std::vector<uint8_t> packet = ccsdsPacket->getAsByteVector();
 * @endcode
 *
 * @subsection example_interpretation Packet interpretation
 * @code
	//constructs an empty instance
	CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();

	//interpret an input data as a CCSDS SpacePacket
	ccsdsPacket->interpret(data,length);

	//check if the packet has Secondary Header
	if(ccsdsPacket->isSecondaryHeaderPresent()){
		...
	}

	//get APID
	std::cout << ccsdsPacket->getPrimaryHeader()->getAPIDAsInteger() << std::endl;

	//dump packet content
	std::cout << ccsdsPacket->toString() << std::endl;
 * @endcode
 *
 * @section feedback Feedback
 * Questions, comments, and requests are welcome.
 * Raise them to
 * <a href="https://github.com/yuasatakayuki/CCSDSLibrary">
 * the CCSDS SpacePacket Library Github site</a>.
 *
 */

/** A class that represents a CCSDS SpacePacket.
 * This class contains structured information of a packet:
 * - Primary Header,
 * - Secondary Header (if present), and
 * - User Data Field.
 * .
 * Primary Header and Secondary Header are represented by dedicated classes
 * CCSDSSpacePacketPrimaryHeader and CCSDSSpacePacketSecondaryHeader, respectively.
 * User Data Field is implemented as std::vector<uint8_t>.
 *
 * @par
 * Example: Packet creation
 * @code
	CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();
	ccsdsPacket->getPrimaryHeader()->setAPID(lowerAPID);
	ccsdsPacket->getPrimaryHeader()->setPacketType(CCSDSSpacePacketPacketType::TelemetryPacket);
	ccsdsPacket->getPrimaryHeader()->setSecondaryHeaderFlag(CCSDSSpacePacketSecondaryHeaderFlag::Present);
	ccsdsPacket->getPrimaryHeader()->setSequenceFlag(CCSDSSpacePacketSequenceFlag::UnsegmentedUserData);
	ccsdsPacket->getSecondaryHeader()->setCategory(category);
	ccsdsPacket->getSecondaryHeader()->
	   setSecondaryHeaderType(CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed);
	ccsdsPacket->getSecondaryHeader()->setADUChannelID(0x00);
	ccsdsPacket->getSecondaryHeader()->setADUSegmentFlag(CCSDSSpacePacketADUSegmentFlag::UnsegmentedADU);
	//set counters
	ccsdsPacket->getPrimaryHeader()->setSequenceCount(sequenceCount);
	ccsdsPacket->getSecondaryHeader()->setADUCount(aduCount);

	//set absolute time
	uint8_t time[4];
	ccsdsPacket->getSecondaryHeader()->setTime(time);

	//set data
	ccsdsPacket->setUserDataField(smcpByteArray);
	ccsdsPacket->setPacketDataLength();

	//get packet as byte array
	std::vector<uint8_t> packet = ccsdsPacket->getAsByteVector();
 * @endcode
 *
 * Example: Packet interpretation
 * @code
	CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();
	ccsdsPacket->interpret(data,length);
	std::cout << ccsdsPacket->toString() << std::endl;
 * @endcode
 *
 * @see CCSDSSpacePacketPrimaryHeader, CCSDSSpacePacketSecondaryHeader
 */
class CCSDSSpacePacket {
public:
	static const uint32_t APIDOfIdlePacket = 0x7FF; //111 1111 1111

public:
	CCSDSSpacePacketPrimaryHeader* primaryHeader;
	CCSDSSpacePacketSecondaryHeader* secondaryHeader;
	std::vector<uint8_t>* userDataField;

public:
	/** Constructs an instance.
	 * Internal instances of the primary header, the secondary header,
	 * and the user data field are constructed.
	 */
	CCSDSSpacePacket() {
		primaryHeader = new CCSDSSpacePacketPrimaryHeader();
		secondaryHeader = new CCSDSSpacePacketSecondaryHeader();
		userDataField = new std::vector<uint8_t>();
		//primaryHeader->setPacketVersionNum(CCSDSSpacePacketPacketVersionNumber::Version1);
	}

public:
	/** Destructor.
	 * Internal instances of the primary header, the secondary header,
	 * and the user data field are deleted.
	 */
	virtual ~CCSDSSpacePacket() {
		delete primaryHeader;
		delete secondaryHeader;
		delete userDataField;
	}

public:
	/** Returns packet content as a vector of uint8_t.
	 * Packet content will be dynamically generated every time
	 * when this method is invoked.
	 * @return a uint8_t vector that contains packet content
	 */
	std::vector<uint8_t> getAsByteVector() {
		calcPacketDataLength();
		std::vector<uint8_t> result;
		std::vector<uint8_t> primaryHeaderVector = primaryHeader->getAsByteVector();
		result.insert(result.end(), primaryHeaderVector.begin(), primaryHeaderVector.end());
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			std::vector<uint8_t> secondaryHeaderVector = secondaryHeader->getAsByteVector();
			result.insert(result.end(), secondaryHeaderVector.begin(), secondaryHeaderVector.end());
		}
		result.insert(result.end(), userDataField->begin(), userDataField->end());
		return result;
	}

public:
	/** Interprets a uint8_t array into this instance.
	 * When the array does not contain a valid CCSDS SpacePacket, an exception may
	 * be thrown.
	 * @param[in] buffer a pointer to a uint8_t array that contains a CCSDS SpacePacket.
	 * @param[in] length the length of the data contained in buffer.
	 */
	void interpret(uint8_t *buffer, size_t length) throw (CCSDSSpacePacketException) {
		using namespace std;

		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::NotACCSDSSpacePacket);
		}
		//primary header
		primaryHeader->interpret(buffer);
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::NotPresent) {
			//buffer field
			userDataField->clear();
			for (size_t i = CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength; i < length; i++) {
				userDataField->push_back(buffer[i]);
			}
		} else {
			//secondary header
			secondaryHeader->interpret(buffer + CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength, //
			length - CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength);
			//buffer field
			userDataField->clear();
			for (size_t i = CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength + secondaryHeader->getLength();
					i < length; i++) {
				userDataField->push_back(buffer[i]);
			}
		}

	}

public:
	/** Interprets data contained in a uint8_t vector into this instance.
	 * @param[in] buffer a uint8_t vector that contains a CCSDS SpacePacket.
	 */
	void interpret(std::vector<uint8_t> & buffer) {
		if (buffer.size() != 0) {
			interpret(&(buffer[0]), buffer.size());
		}
	}

public:
	/** Interprets data contained in a uint8_t vector into this instance.
	 * @param[in] buffer a pointer to a uint8_t vector that contains a CCSDS SpacePacket.
	 */
	void interpret(std::vector<uint8_t>* buffer) {
		if (buffer->size() != 0) {
			interpret(&( (*buffer)[0]), buffer->size());
		}
	}

private:
	void calcPacketDataLength() {
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			primaryHeader->setPacketDataLength(secondaryHeader->getAsByteVector().size() + userDataField->size() - 1);
		} else {
			primaryHeader->setPacketDataLength(userDataField->size() - 1);
		}
	}

public:
	/** Fill Packet Data Length field with an appropriate number
	 * which is automatically calculated by this class.
	 */
	void setPacketDataLength() {
		calcPacketDataLength();
	}

public:
	/** Sets uint8_t array to User Data Field.
	 */
	void setUserDataField(std::vector<uint8_t> *userDataField) {
		*(this->userDataField) = *userDataField;
		setPacketDataLength();
	}

public:
	/** Sets uint8_t array to User Data Field.
	 */
	void setUserDataField(std::vector<uint8_t> userDataField) {
		*(this->userDataField) = userDataField;
		setPacketDataLength();
	}

public:
	/** Returns string dump of this packet.
	 * @returns string dump of this packet.
	 */
	virtual std::string toString() {
		std::stringstream ss;
		using std::endl;
		ss << "---------------------------------" << endl;
		ss << "CCSDSSpacePacket" << endl;
		ss << "---------------------------------" << endl;
		ss << primaryHeader->toString();
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			ss << secondaryHeader->toString();
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

public:
	/** Dumps self as string to a provided output stream.
	 * @param[in] os output stream such as fstream or cout.
	 */
	virtual void dump(std::ostream& os) {
		os << this->toString();
	}

public:
	/** Dumps self as string to a provided output stream.
	 * @param[in] os output stream such as fstream or cout.
	 */
	virtual void dump(std::ostream* os) {
		*os << this->toString();
	}

public:
	/** Dumps self as string to the screen.
	 */
	virtual void dumpToScreen() {
		dump(std::cout);
	}

public:
	/** A utility method which converts std::vector<uint8_t> to
	 * std::string.
	 */
	static std::string arrayToString(std::vector<uint8_t> *data, std::string mode = "dec",
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
				ss << dec << left << (uint32_t) (data->at(i));
			} else if (mode == "hex") {
				ss << hex << "0x" << setw(2) << setfill('0') << right << (uint32_t) (data->at(i));
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

public:
	/** Checks if Secondary Header is present.
	 */
	inline bool isSecondaryHeaderPresent(){
		if(primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present){
			return true;
		}else{
			return false;
		}
	}

public:
	/** Checks if Secondary Header is present.
	 */
	inline bool hasSecondaryHeader(){
		return isSecondaryHeaderPresent();
	}

public:
	/** Checks if Secondary Header is present.
	 */
	inline bool isSecondaryHeaderUsed(){
		return isSecondaryHeaderPresent();
	}

public:
	/** Returns a Primary Header instance.
	 * @returns a pointer to the Primary Header of this packet.
	 */
	inline CCSDSSpacePacketPrimaryHeader* getPrimaryHeader() const {
		return primaryHeader;
	}

public:
	/** Returns a Secondary Header instance.
	 * @returns a pointer to the Secondary Header of this packet.
	 */
	inline CCSDSSpacePacketSecondaryHeader* getSecondaryHeader() const {
		return secondaryHeader;
	}

public:
	/** Returns a User Data Field pointer (std::vector<uint8_t>*).
	 * @returns a pointer to the User Data Field of this packet.
	 */
	inline std::vector<uint8_t>* getUserDataField(){
		return userDataField;
	}
};

#endif /* CCSDSSPACEPACKET_HH_ */
