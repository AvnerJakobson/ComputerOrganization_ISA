000  	out $zero, $zero, $imm2, $imm1, 1, 2		# enable irq2
001  	sll $sp, $imm1, $imm2, $zero, 1, 11		# set $sp = 1 << 11 = 2048
002  	out $zero, $imm1, $zero, $imm2, 6, L3		# set irqhandler as L3
003  	lw $a0, $zero, $imm2, $zero, 0, 64		# get x from address 64
004  	jal $ra, $zero, $zero, $imm2, 0, fib		# calc $v0 = fib(x)
005  	sw $zero, $zero, $imm2, $v0, 0, 65		# store fib(x) in 65
006  	halt $zero, $zero, $zero, $zero, 0, 0		# halt
007  fib:
007  	add $sp, $sp, $imm2, $zero, 0, -3		# adjust stack for 3 items
008  	sw $zero, $sp, $imm2, $s0, 0, 2			# save $s0
009  	sw $zero, $sp, $imm2, $ra, 0, 1			# save return address
00A  	sw $zero, $sp, $imm2, $a0, 0, 0			# save argument
00B  	bgt $zero, $a0, $imm1, $imm2, 1, L1		# jump to L1 if x > 1
00C 	add $v0, $a0, $zero, $zero, 0, 0		# otherwise, fib(x) = x, copy input
00D  	beq $zero, $zero, $zero, $imm2, 0, L2		# jump to L2
00E  L1:
00E	 	sub $a0, $a0, $imm2, $zero, 0, 1		# calculate x - 1
00F  	jal $ra, $zero, $zero, $imm2, 0, fib		# calc $v0=fib(x-1)
010  	add $s0, $v0, $imm2, $zero, 0, 0		# $s0 = fib(x-1)
011  	lw $a0, $sp, $imm2, $zero, 0, 0			# restore $a0 = x
012  	sub $a0, $a0, $imm2, $zero, 0, 2		# calculate x - 2
013  	jal $ra, $zero, $zero, $imm2, 0, fib		# calc fib(x-2)
014  	add $v0, $v0, $s0, $zero, 0, 0			# $v0 = fib(x-2) + fib(x-1)
015  	lw $a0, $sp, $imm2, $zero, 0, 0			# restore $a0
016  	lw $ra, $sp, $imm2, $zero, 0, 1			# restore $ra
017  	lw $s0, $sp, $imm2, $zero, 0, 2			# restore $s0
018  L2:
018  	add $sp, $sp, $imm2, $zero, 0, 3		# pop 3 items from stack
019  	add $t0, $a0, $zero, $zero, 0, 0		# $t0 = $a0
01A  	sll $t0, $t0, $imm2, $zero, 0, 16		# $t0 = $t0 << 16
01B  	add $t0, $t0, $v0, $zero, 0, 0			# $t0 = $t0 + $v0
01C  	out $zero, $zero, $imm2, $t0, 0, 10		# write $t0 to display
01D  	beq $zero, $zero, $zero, $ra, 0, 0		# and return
01E  L3:
01E  	in $t1, $zero, $imm2, $zero, 0, 9		# read leds register into $t1
01F  	sll $t1, $t1, $imm2, $zero, 0, 1		# left shift led pattern to the left
020  	or $t1, $t1, $imm2, $zero, 0, 1			# lit up the rightmost led
021  	out $zero, $zero, $imm2, $t1, 0, 9		# write the new led pattern
022  	out $zero, $zero, $imm2, $imm1, 255, 21		# set pixel color to white
023  	out $zero, $zero, $imm2, $imm1, 1, 22		# draw pixel
024  	in $t1, $zero, $imm2, $zero, 0, 20		# read pixel address
025  	add $t1, $t1, $imm2, $zero, 0, 257		# $t1 += 257
026  	out $zero, $zero, $imm2, $t1, 0, 20		# update address
027  	out $zero, $zero, $imm2, $zero, 0, 5		# clear irq2 status
028  	reti $zero, $zero, $zero, $zero, 0, 0		# return from interrupt
029  	.word 64 7
02A
02B
02C
02D
