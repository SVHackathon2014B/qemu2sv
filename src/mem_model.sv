`timescale 1ns/1ns

module hoge();
	parameter MEM_SIZE = 1000;

	typedef struct {
		int offset;
		int data;
		int rw;
	} packet_t;

	import "DPI-C" context function void init_socket( void );
	import "DPI-C" context function void recieve_packet( output trans );
	import "DPI-C" context function void send_packet( packet_t trans );

	int mem[MEM_SIZE];
	packet_t trans;
	
	initial begin
		init_socket();
	end
	
	
	
	/*
    initial begin
		while(1) begin
			#10;
			recieve_packet( trans );
			if ( trans.rw == 1) begin
				mem[trans.offset] <= trans.data;
				$display("$t : mem[%d] = %d", trans.offset, mem[trans.offset] );
			end else begin
				send_packet( trans );
			end 
		end
    end
	*/
	
endmodule