all : interpret_ccsds_packet

interpret_ccsds_packet : interpret_ccsds_packet.cc
	g++ -I../includes interpret_ccsds_packet.cc -o interpret_ccsds_packet

clean :
	rm -f interpret_ccsds_packet
