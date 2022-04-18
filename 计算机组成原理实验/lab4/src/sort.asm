.data
.word 0x814d0bcf
.word 0x900b07d1
.word 0xc5246b59
.word 0x85f34240
.word 0xf0648f06
.word 0xa726af47
.word 0x59e970a2
.word 0x79f57ff2
.word 0x4dd57a20
.word 0x6ffe43a2
.word 0x8eee87a2
.word 0xa96ab6ff
.word 0x3bbbd029
.word 0xaf1b67c7
.word 0xaf3401e6
.word 0x35336804

READY: .word 0x0000ff08
DATA:  .word 0x0000ff0c

.text
addi	t1, zero, 0	# sorted
addi	t0, zero, 17	# set n to array size + 1
addi 	a1, zero, 16	# const size (for print out
addi	a0, zero, 1	# const 1
OUTER:
addi	t0, t0, -1
sub	t1, a0, t1
beqz	t1, END
addi	t2, zero, 0	# i = 0
INNER:
addi	t2, t2, 1	# init: i = 1
beq	t2, t0, OUTER
addi	t5, t2, -1
add	t5, t5, t5
add	t5, t5, t5
lw	t3, (t5)
add	t6, t2, t2
add	t6, t6, t6
lw	t4, (t6)
blt	t3, t4, INNER
beq	t3, t4, INNER	# 否则需要交换
sw	t3, (t6)
sw	t4, (t5)
addi	t1, zero, 0
j	INNER
END:
# 遍历数组
addi	t0, zero, 0
DISP:
# 如果在范围内，那么取出这个数，存到 t2
beq	t0, a1, FINISH
add	t2, t0, t0
add	t2, t2, t2
lw	t2, (t2)
# t2 已经存这个数，等待 ready bit 为 1 时写入数据
WAIT:
lw	t1, READY
lw	t1, (t1)
beqz	t1, WAIT
lw	t1, DATA
sw	t2, (t1)
addi	t0, t0, 1
j DISP
FINISH:
