main:
	lw $a0, $imm1, $zero, $zero, 0x100, 0 		# load n
	lw $a1, $imm1, $zero, $zero, 0x101, 0 		# load k
	add $sp, $zero, $imm2, $zero, 0, 2000		# set stack at 2000
	add $v0, $imm1, $zero, $zero, 0, 0 			# set v0 to 0
	jal $ra, $zero, $zero, $imm1, binom, 0	 	# binom(n, k)
	sw $v0, $zero, $imm2, $zero, 0, 0X102 		# save result to 0x102, uppercase hexa number for debug
	halt $zero, $zero, $zero, $zero, 0, 0		# halt
	
binom:
	beq $zero, $a0, $zero, $imm1, return_one, 0 # if n == 0, return_one
	beq $zero, $a0, $a1, $imm1, return_one, 0 	# if n == k, return_one
	add $sp, $sp, $imm2, $zero, 0, -4			# adjust stack for 4 items
	sw $zero, $sp, $imm2, $a0, 0, 3				# save $a0
	sw $zero, $sp, $imm2, $a1, 0, 2 			# save $a1
	sw $zero, $sp, $imm2, $ra, 0, 1				# save return address
	sw $zero, $sp, imm2, $s0, 0, 0 				# save $s0
	add $a0, $a0, $imm1, $zero, -1, 0 			# else, set n = n-1
	jal $ra, $zero, $zero, $imm1, binom, 0	 	# binom(n-1, k)
	add $s0, $v0, $zero, $zero, 0, 0 			# save recursion result to $s0
	lw $a0, $sp, $imm1, $imm2, 3, -1 			# load n-1
	lw $a1, $sp, $imm1, $imm2, 2, -1 			# load k-1
	jal $ra, $zero, $imm1, binom, 0	 			# binom(n-1, k-1)
	add $v0, $v0, $s0, $zero, 0, 0 				# sum recursion results
	jal $zero, $zero, $zero, $imm1, END, 0 		# jump to END, writes address to $zero for debug testing
	
return_one:
	add $v0, $imm1, $zero, $zero, 0, 0 			# v0 = 1
	jal $t0, $zero, $zero, $imm1, END, 0 		# jump to END

END:
	lw $s0, $sp, $imm2, $zero, 0, 0				# restore $s0
	lw $ra, $sp, $imm2, $zero, 0, 1				# restore $ra
	lw $a1, $sp, $imm2, $zero, 0, 2				# restore $a1
	lw $a0, $sp, $imm2, $zero, 0, 3 			# restore $a0
	add $sp, $sp, $imm2, $zero, 0, 4			# adjust stack pointer
	jal $t0, $zero, $zero, $ra, 0, 0 			# return to caller