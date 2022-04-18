.data
NUMBER0:  .word  0xffffffff		# 第一个字
NUMBER1:  .word  0x000000ff		# 第二个字

.text
lw	t0, NUMBER0
lw	t1, NUMBER1
add 	t2, t0, t1
addi	t2, t2, -0xff
SUB:
sub	t3, t0, t1
beq	t0, t1, SUB
beq	t0, t0, NEXT
nop
NEXT:
auipc	t4, 16
auipc   t6, 0
addi	t6, t6, 24
blt	t1, t0, NEXT2
blt	t0, t1, NEXT2
NEXT2:
jal	t5 NEXT3
NEXT3:
jalr 	t6			# 应该跳转到下一条指令
sw	t1, NUMBER0, t6
sw	t0, NUMBER1, t6
