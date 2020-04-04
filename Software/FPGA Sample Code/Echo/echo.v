module echo #(
	parameter BITSIZE = 16
)(	
	input wire enable,

	input wire bclk,    // 64 times lrclk 
	input wire lrclk,

	input wire signed [BITSIZE-1:0] in,
	output wire [BITSIZE-1:0] out,
);

if (BITSIZE == 24) begin
    $error("ECHO SUPPORT FOR 24 NOT AVAILABLE YET");
end

localparam ADDRLEN = 14;

reg [ADDRLEN-1:0] rd_ptr = 1;
reg [ADDRLEN-1:0] wr_ptr = 0;

reg [2:0] counter = 0;

reg wren;
reg [ADDRLEN-1:0] memaddr;
reg signed [BITSIZE-1:0] datain;
reg signed [BITSIZE-1:0] dataout;
wire signed [BITSIZE-1:0] outbuff;

// memory  #( 
//    .BITSIZE(BITSIZE),
//    .ADDRLEN(ADDRLEN),
// ) M1 (
//     .clk(bclk),
//     .addr(memaddr),
//     .datain(datain),
//     .dataout(dataout),
// 	.wren(wren)
// );

SB_SPRAM256KA M1 (
  .ADDRESS (memaddr), 
  .DATAIN (datain), 
  .DATAOUT (outbuff),
  .MASKWREN (4'b1111),
  .WREN (wren),
  .CHIPSELECT (1'b1),
  .CLOCK (bclk),
  .STANDBY (1'b0),
  .SLEEP (1'b0),
  .POWEROFF (1'b1)
);

always @(posedge lrclk) begin
	if (enable) begin
		out <= (in >>> 1) + (dataout >>> 1);
	end else begin
		out <= in;
	end

end

always @(posedge bclk) begin

	if(lrclk)
		counter <= 1;
		
	if (counter == 1) begin
		datain <= (in >>> 2) +  (dataout >>> 2);
		wren <= 1;
		memaddr <= wr_ptr;
		counter <= counter + 1;
	end else if (counter == 2) begin
		wren <= 0;
		memaddr <= rd_ptr;
		counter <= counter + 1;
	end else if (counter == 3) begin
		dataout <= outbuff;
		wr_ptr <= wr_ptr + 1;
		rd_ptr <= rd_ptr + 1;
		counter <= counter + 1;
	end
end

endmodule