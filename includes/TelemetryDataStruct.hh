/*
 * TelemetryDataStruct.hpp
 *
 *  Created on: 2013/01/24
 *      Author: sakurai
 */

#ifndef TELEMETRYDATASTRUCT_HPP_
#define TELEMETRYDATASTRUCT_HPP_

#include <iostream>
#include <vector>
#include <algorithm>

namespace sib2 {

typedef std::vector<uint8_t> TelemetryData;
typedef std::vector<uint8_t> CCSDSPacketData;

struct ID{
	uint16_t upperAPID;
	uint16_t lowerAPID;
	uint16_t ADUChannelID;
	uint8_t ADUCount;
	uint32_t TI;
	uint16_t lowerFOID;
	uint16_t attributeID;
	ID(){
		upperAPID = 0;
		lowerAPID = 0;
		lowerFOID = 0;
		attributeID = 0;
		ADUChannelID = 0;
		ADUCount = 0;
		TI = 0;
	}
	uint16_t getFOID() const{
		return (lowerFOID + (lowerAPID<<8));
	}
	void dump(std::ostream& ost){
		ost<<std::hex<<"LowerAPID="<<lowerAPID<<", LowerFOID="<<lowerFOID<<", AttributeID="<<attributeID<<", ADUChannelID="<<ADUChannelID<<", ADUCount="<<ADUCount<<", TI="<<TI<<std::endl;
	}
	bool operator ==(ID id){
		return (upperAPID==id.upperAPID&&lowerAPID==id.lowerAPID&&lowerFOID==id.lowerFOID&&attributeID==id.attributeID
				&&ADUChannelID==id.ADUChannelID && ADUCount==id.ADUCount);
	}
	bool operator !=(ID id){
		return (upperAPID!=id.upperAPID||lowerAPID!=id.lowerAPID||lowerFOID!=id.lowerFOID||attributeID!=id.attributeID
				||ADUChannelID!=id.ADUChannelID || ADUCount!=id.ADUCount);
	}
	static const ID& NA(){
		static const ID NAID;
		return NAID;
	}
};


class SMCPPacketData{
public:
	SMCPPacketData(){
		packettype = 0;
		upperAPID = 0;
		lowerAPID = 0;
		ADUChannelID = 0;
		ADUCount = 0;
		TI = 0;
	}
	uint16_t packettype;
	uint16_t upperAPID;
	uint16_t lowerAPID;
	uint16_t ADUChannelID;
	uint8_t ADUCount;
	uint32_t TI;
	bool isTMPacket() const{
		return packettype==0;
	}
	bool isTCPacket() const{
		return packettype==1;
	}
	std::vector<uint8_t> data;
};


struct UserData{
//	uint16_t upperAPID;
//	uint16_t lowerAPID;
//	uint16_t lowerFOID;
//	uint16_t attributeID;
	ID id;
	std::vector<uint8_t> data;
	void clear(){
		id.upperAPID = 0;
		id.lowerAPID = 0;
		id.lowerFOID = 0;
		id.attributeID = 0;
		id.ADUChannelID = 0;
		id.ADUCount = 0;
		data.clear();
	}
	void operator =(std::vector<uint8_t>& val){
		data = val;
	}
};

}

#endif /* TELEMETRYDATASTRUCT_HPP_ */
