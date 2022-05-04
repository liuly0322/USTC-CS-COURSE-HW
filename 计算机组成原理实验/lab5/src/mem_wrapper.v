module mem_wrapper (
  a,
  d,
  dpra,
  clk,
  en,
  we,
  spo,
  dpo,
  io_addr,
  io_dout,
  io_we,
  io_rd,
  io_din
);

input wire [31 : 0] a;
input wire [31 : 0] d;
input wire [9 : 0] dpra;
input wire clk;
input wire en;
input wire we;
output wire [31 : 0] spo;
output wire [31 : 0] dpo;

// 外设地址范围： 0x00, 0x04, 0x08, 0x0c, 0x10, 0x14, 0x18
output [7:0]  io_addr;	// 外设地址
output [31:0]  io_dout;	// 向外设输出的数据
output  io_we;		    // 向外设输出数据时的写使能信号
output  io_rd;		    // 从外设输入数据时的读使能信号
input [31:0]  io_din;   // 来自外设输入的数据

wire is_mmio = (a[15:8] == 8'hff);    // 判断现在是主存还是 mmio
wire [31:0] mem_out;
assign spo = is_mmio? io_din : mem_out;
dist_mem data_mem (
  .a(a[9:2]),
  .d(d),
  .dpra(dpra[9:2]),
  .clk(clk),
  .we(we),
  .spo(mem_out),
  .dpo(dpo)
);

assign io_addr = a[7:0];
assign io_dout = d;
assign io_we   = we & is_mmio;
assign io_rd   = en & is_mmio;

endmodule
