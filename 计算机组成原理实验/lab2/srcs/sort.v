module sort (
        input  clk,         // clk
        input  rstn,        // cpu_rstn
        input [15:0]  sw,	// sw, 输入一位数字
        input  del,		    // btnl, 删除一位数字
        input  set_addr,	// btnu, 保存地址
        input  set_data,	// btnc, 保存数据
        input  chk,		    // btnr, 切换到下一个地址
        input  run,		    // btnd, 启动排序
        output [7:0]  an,	// 数码管显示存储器地址和数据
        output [6:0]  seg,  // 输出到数码管
        output busy,		// 标记是否正在排序
        output [15:0]  cnt	// 排序耗费时钟周期数
    );
    // 状态机主要是为了 RAM 使用，输入输出模式也需要 a, d, we 以写入数据
    parameter S_IO     = 1'b0;		// 等待数据写入，初始状态
    parameter S_SORT   = 1'b1;		// 排序状态
    reg cur_state, nxt_state;

    wire [7:0] a, a_IO, a_sort;
    wire [15:0] d, d_IO, d_sort;
    wire we, we_IO, we_sort;
    wire [7:0] dpra;
    wire [15:0] spo, dpo;          // 这些不存在竞争问题
    assign a = cur_state == S_IO? a_IO : a_sort;
    assign d = cur_state == S_IO? d_IO : d_sort;
    assign we = cur_state == S_IO? we_IO : we_sort;
    dist_mem mem (
                 .a(a),        // read/write address input
                 .d(d),        // write data input
                 .dpra(dpra),  // read address input
                 .clk(clk),    // input wire clk
                 .we(we),      // input wire we
                 .spo(spo),    // output wire [15 : 0] spo
                 .dpo(dpo)    // output wire [15 : 0] dpo
             );

    wire run_IO;    // 代表进入排序状态
    io i_o (.clk(clk), .rstn(rstn), .sw(sw), .del(del), .set_addr(set_addr), .set_data(set_data), .chk(chk), .run(run), .sort_end(sort_end), .run_IO(run_IO), .an(an), .seg(seg), .busy(busy), .cnt(cnt), .a(a_IO), .d(d_IO), .we(we_IO), .spo(spo));

    wire sort_end;
    sort_module s_m (.clk(clk), .rstn(rstn), .run(run_IO), .spo(spo), .dpo(dpo), .a(a_sort), .d(d_sort), .we(we_sort), .dpra(dpra), .finish(sort_end));

    always @(posedge clk) begin
        if (!rstn)
            cur_state <= S_IO;
        else
            cur_state <= nxt_state;
    end

    always @(*) begin
        case (cur_state)
            S_IO: begin
                if (run_IO)
                    nxt_state = S_SORT;
                else
                    nxt_state = S_IO;
            end
            S_SORT: begin
                if (sort_end)
                    nxt_state = S_IO;
                else
                    nxt_state = S_SORT;
            end
        endcase
    end

endmodule
