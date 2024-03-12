
out $zero, $zero, $zero, $imm2, 0, 1 					# irq0enable (IO0) = 1
out $zero, $zero, $imm1, $imm2, 13, 16	 				# timer_max(IO13) = 16
out $zero, $zero, $imm1, $imm2, 11, 1 					# timer_enable = 1
out $zero, $imm1, $zero, $imm2, 6, timer_isr 			# irq_handler(IO6) = timer_isr

inf_loop:
	add $t0, $zero, $zero, $zero, 0, 0 					# $t0 = 0
	beq $zero, $zero, $zero, $imm2, 0, inf_loop			# jump to inf_loop
	

timer_isr:
	add $t1, $imm2, $zero, $zero, 0, -1000 				# $t1 = -1000
	add $t2, $imm2, $zero, $zero, 0, 2					# $t2 = 2
	sll $t2, $t2, $imm2, $zero, 0, 1 					# sll $t2 by 1
	srl $t2, $t2, $imm2, $zero, 0, 1 					# srl $t2 by 1
	sra $t1, $t1, $imm2, $zero, 0, 1 					# sra $t1 by 1
	xor $s0, $t2, $imm2, $zero, 0, 6 					# xor $t2 with 6
	and $s0, $t2, $imm2, $zero, 0, 6 					# and $t2 with 6
	or $s2, $imm1, $imm2, $zero, 1, 14 					# or 1 and 14 in $s2
	out $zero, $zero, $imm1, $imm2, 3, 0				# set irq0status (IO3) t0 0
	halt $zero, $zero, $zero, $zero, 0, 0 				# halt
	
	