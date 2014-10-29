typedef struct {   
    int unsigned offset; 
	int unsigned data; 
	int we;      
} rw_packet_t; 

module mem_model();
	import "DPI-C" context function int init_socket();
	import "DPI-C" context function int socket_start();
	export "DPI-C" function write_access;
	export "DPI-C" function read_access;
	
	
    int ret;
	rw_packet_t rw_packet1;
	rw_packet_t rw_packet2;
	
	logic reset_n;
	logic clock;
	
	int unsigned hash_mem[int unsigned];
	
	//clock
	initial
		clock = 0;
	
	always #5
		clock = ~clock;
	
	
	//reset & init_socket
	initial begin
		reset_n = 0;
		ret = init_socket();
		if (ret == 1) begin
			$display("SV::connection timeout\n");
		end
		
		repeat(10) @(posedge clock);
		reset_n = 1;
		while (ret == 0) begin
			ret = socket_start();
		end
		
		$display("SV::connection faild\n");
		
		$finish;
	end
	
	function void write_access(input rw_packet_t rw_packet);
		$display("sv::write::rw_packet.offset = 0x%x rw_packet.data = 0x%x\n",rw_packet.offset,rw_packet.data);
		hash_mem[rw_packet.offset >> 2] = rw_packet.data;
	endfunction
	
	function longint read_access(input rw_packet_t rw_packet);
		$display("sv::read::rw_packet.offset = 0x%x rw_packet.data = 0x%x\n",rw_packet.offset,hash_mem[rw_packet.offset >> 2]);
		read_access = hash_mem[rw_packet.offset >> 2];
	endfunction

endmodule
