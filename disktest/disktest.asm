main:
	# set irq1enable to 1
	# $t0 = 8, holds the disksector 
	# $t1 = 0, holds the 

loop:
	# check if diskstatus = 0
	# diskcmd = 1, read
	# wait for diskstatus = 0?
	# 
	# diskcmd = 2, write to disksector + 1
	# disksector-- 
	# if $t0 < 0, jump to END



ISR:
	add $sp, $sp, $imm2, $zero, 0, -4				# adjust stack for 4 items
	sw $zero, $sp, $imm2, $s0, 0, 3					# save $s0
	sw $zero, $sp, $imm2, $s1, 0, 2					# save $s1
	sw $zero, $sp, $imm2, $s2, 0, 1					# save $s2
	sw $zero, $sp, $immw, $a0 0, 0 					# save $a0
	#
	# if diskcmd == 1
	# else if diskcmd == 2
	# else
	# reti
	

END:

