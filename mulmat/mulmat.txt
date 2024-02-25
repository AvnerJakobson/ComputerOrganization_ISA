main:
	add $sp, $sp, $imm2, $zero, 0, -3				# adjust stack for 3 items
	sw $zero, $sp, $imm2, $s0, 0, 2					# save $s0
	sw $zero, $sp, $imm2, $s1, 0, 1					# save $s1
	sw $zero, $sp, $imm2, $s2, 0, 0					# save $s2
	add $a0, $zero, $imm2, $zero, 0, 0x100			# get first matrix, first row address to a0
	add $a1, $zero, $imm2, $zero, 0, 0x110			# get second matrix, first column address to a1
	add $s0, $zero, $imm2, $zero, 0, 0x120			# result matrix first element address
	jal $ra, $zero, $zero, $imm2, 0, mult_vec 		# get one result
	sw $zero, $s0, $v0, $zero, 0, 0 				# set res_mat[elem] to v0
	add $s1, $zero, $imm2, $zero, 0, 1				# set row counter to 1
	add $s2, $zero, $imm2, $zero, 0, 1				# set col counter to 1

a1_loop:
	add $s2, $s2, $imm2, $zero, 0, 1 				# increment col counter by 1
	add $s0, $s0, $imm2, $zero, 0, 1 				# increment res_mat element by 1
	add $a1, $a1, $imm2, $zero, 0, 1				# increment a1 by 1
	jal $ra, $zero, $zero, $imm2, 0, mult_vec		# get result
	sw $v0, $s0, $zero, $zero, 0, 0					# set res_mat[elem] to v0
	mac $t0, $s1, $s2, $zero, 0, 0					# set $t0 to counter*counter
	beq $zero, $t0, $imm1, $imm2, 16, END			# if col counter == 4 and row counter == 4, jump to END
	beq $zero, $s2, $imm1, $imm2, 4, a0_loop		# else, if col_counter == 4 jump to a0_loop, meaning we finished the row
	beq $zero, $zero, $zero, $imm2, 0, a1_loop		# else, keep looping over columns
a0_loop:
	add $s2, $zero, $zero, $zero, 0, 0				# set col counter to 0
	add $a1, $zero, $imm2, $zero, 0, 0x10F 		# set a1 to 0x10F, so that a1 + 1 = 0x110
	add $s1, $s1, $imm2, $zero, 0, 1 				# increment row counter by 1
	add $a0, $a0, $imm2, $zero, 0, 4 				# increment a0 by 4
	beq $zero, $zero, $zero, $imm2, 0, a1_loop		# jump to a1_loop
	
mult_vec:
	add $t2, $zero, $imm2, $zero, 0, 1  			# set counter to 1 
	lw $t0, $a0, $zero, $zero, 0, 0					# load first elem
	lw $t1, $a1, $zero, $zero, 0, 0					# load second elem
	mac $v0, $t0, $t1, $zero, 0, 0 					# multiply elems
inner_loop:
	add $t2, $t2, $imm2, $zero, 0, 1  				# increment counter by 1 
	lw $t0, $a0, $imm1, $zero, 1, 0  				# increment a0 by 1 and load
	lw $t1, $a1, $imm1, $zero, 4, 0 				# increment a1 by 4 and load
	mac $v0, $t0, $t1, $v0, 0, 0 					# multiply and sum with pervious results
	beq $zero, $t2, $imm1, $ra, 4, 0				# if counter == 4: return 
	beq $zero, $zero, $zero, $imm2, 0, inner_loop	# else, keep incrementing
	
END:
	lw $s2, $sp, $imm2, $zero, 0, 0					# restore $s2
	lw $s1, $sp, $imm2, $zero, 0, 1					# restore $s1
	lw $s0, $sp, $imm2, $zero, 0, 2					# restore $s0
	add $sp, $sp, $imm2, $zero, 0, 3				# adjust stack pointer