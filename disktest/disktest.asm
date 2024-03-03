main:
	# set irq0enable to 1
	# set irq1enable to 1
	# $t0 = 8, holds the disksector 
	# $t1 = 0, holds the diskbuffer

loop:
	# check if diskstatus = 0
	# diskcmd = 1, read
	# wait for diskstatus = 0?
	# 
	# diskcmd = 2, write to disksector + 1
	add $t0, $t0, $imm1, $zero, -1, 0 				# disksector-- 
	# if $t0 < 0, jump to END



ISR:
	add $sp, $sp, $imm2, $zero, 0, -3				# adjust stack for 3 items
	sw $zero, $sp, $imm2, $s0, 0, 2					# save $s0
	sw $zero, $sp, $imm2, $s1, 0, 1					# save $s1
	sw $zero, $sp, $imm2, $s2, 0, 0					# save $s2
	# load irq0
	# check if irq0 == 1
	# load irq1
	
	# if diskcmd == 1
	# else if diskcmd == 2
	# else
	# 
	# reti
	

END:
	lw $s2, $sp, $imm2, $zero, 0, 0					# restore $s2
	lw $s1, $sp, $imm2, $zero, 0, 1					# restore $s1
	lw $s0, $sp, $imm2, $zero, 0, 2					# restore $s0
	add $sp, $sp, $imm2, $zero, 0, 3				# adjust stack pointer
	halt $zero, $zero, $zero, $zero, 0, 0			# halt
