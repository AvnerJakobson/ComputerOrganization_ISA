main:
	add $sp, $zero, $imm2, $zero, 0, 2000				# set stack at 2000
	out $zero, $imm1, $zero, $imm2, 6, ISR				# irqhandler PC = ISR
	out $zero, $imm1, $zero, $imm2, 1, 1 				# set irq1enable to 1
	out $zero, $imm1, $zero, $imm2, 15, 7 				# disksector = 7
	out $zero, $imm1, $zero, $imm2, 12, 500 			# diskbuffer = 500

loop:
	jal $ra, $zero, $zero, $imm1, wait_for_disk, 0 		# wait_for_disk to be free
	out $zero, $imm1, $zero, $imm2, 14, 1				# diskcmd = 1, read
	in $t0, $imm1, $zero, $zero, 15, 0 					# $t0 = disk sector
	add $t0, $t0, $imm1, $zero, 1, 0 					# $t0 ++
	out $zero, $imm1, $zero, $t0, 15, 0 				# disksector = $t0
	jal $ra, $zero, $zero, $imm1, wait_for_disk, 0 		# wait_for_disk to be free
	out $zero, $imm1, $zero, $imm2, 14, 2 				# diskcmd = 2, write to disksector + 1
	jal $ra, $zero, $zero, $imm1, wait_for_disk, 0 		# wait_for_disk to be free
	add $t0, $t0, $imm1, $zero, -2, 0 					# t0 -= 2 
	blt $zero, $t0, $imm1, $imm2, 0, END				# if $t0 < 0, jump to END
	out $zero, $imm1, $zero, $t0, 15, 0 				# disksector = $t0
	beq $zero, $zero, $zero, $imm1, loop, 0 			# return to loop

wait_for_disk:
	in $t1, $imm1, $zero, $zero, 17, 0 					# $t1 = diskstatus
	beq $zero, $t1, $imm1, $ra, 0, 0 					# if diskstatus == 0, return to the loop thru $ra
	beq $zero, $zero, $zero, $imm1, wait_for_disk, 0 	# jump to wait_for_disk

ISR:
	out $zero, $imm1, $zero, $imm2, 17, 0 				# set diskstatus to 0
	out $zero, $imm1, $zero, $imm2, 4, 0 				# set irq1status(IO4) to 0
	reti $zero, $zero, $zero, $zero, 0, 0				# return from interrupt
	
END:
	halt $zero, $zero, $zero, $zero, 0, 0				# halt