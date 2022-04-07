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

READY: .word 0x00007f08
DATA:  .word 0x00007f0c

.text
andi	t1, t1, 0	# sorted
addi	t0, zero, 17	# set n to array size + 1
OUTER:
addi	t0, t0, -1
xori	t1, t1, 1
beqz	t1, END
addi	t2, zero, 0	# i = 0
INNER:
addi	t2, t2, 1	# init: i = 1
bge	t2, t0, OUTER
addi	t5, t2, -1
slli	t5, t5, 2
lw	t3, (t5)
slli	t6, t2, 2
lw	t4, (t6)
ble	t3, t4, INNER	# 否则需要交换
sw	t3, (t6)
sw	t4, (t5)
addi	t1, zero, 0
j	INNER
j	OUTER
END:
# 遍历数组，ready bit 为 1 时写入数据
andi	t0, t0, 0	# t0 =  &a[0]
DISP:
# 小于 64, 则取出这个数，存到 t5
srli	t4, t0, 6
bnez	t4, FINISH
lw	t5, (t0)
# 依次右移 28, 24, ...0 后取低 4 位
addi 	t3, zero, 28
BYTE:
blt	t3, zero, LF
srl	t4, t5, t3
andi	t4, t4, 15
# 根据 t4 的值判断写出什么
addi	t6, zero, 10
blt	t4, t6, NUMBER
# 这里写出 a 到 f
addi	t4, t4, 87
j	DISBYTE
NUMBER:
# 这里写出数字
addi	t4, t4, 48
DISBYTE:
# t4 已经存 ascii，等待 READY
WAIT:
lw	t1, READY
lw	t1, (t1)
beqz	t1, WAIT
lw	t1, DATA
sw	t4, (t1)
addi	t3, t3, -4
j BYTE
# 写入换行
LF:
lw	t1, READY
lw	t1, (t1)
beqz	t1, LF
addi	t2, zero, 10
lw	t1, DATA
sw	t2, (t1)
addi	t0, t0, 4	# next number
j	DISP
FINISH:
nop
