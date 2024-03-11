init_mat_1:
	.word 0x100 1 											# set a(0, 0)
	.word 0x101 0					 						# set a(0, 1)
	.word 0x102 0 											# set a(0, 2)
	.word 0x103 0					 						# set a(0, 3)
	.word 0x104 0 											# set a(1, 0)
	.word 0x105 1 					 						# set a(1, 1)
	.word 0x106 0 											# set a(1, 2)
	.word 0x107 0					 						# set a(1, 3)
	.word 0x108 0 											# set a(2, 0)
	.word 0x109 0					 						# set a(2, 1)
	.word 0x10A 1 											# set a(2, 2)
	.word 0x10B 0					 						# set a(2, 3)
	.word 0x10C 0 											# set a(3, 0)
	.word 0x10D 0					 						# set a(3, 1)
	.word 0x10E 0 											# set a(3, 2)
	.word 0x10F 1					 						# set a(3, 3)
	
init_mat_2:
	.word 0x110 1 											# set b(0, 0)
	.word 0x111 2 					 						# set b(0, 1)
	.word 0x112 3 											# set b(0, 2)
	.word 0x113 4 					 						# set b(0, 3)
	.word 0x114 1 											# set b(1, 0)
	.word 0x115 2 						 					# set b(1, 1)
	.word 0x116 3 											# set b(1, 2)
	.word 0x117 4						 					# set b(1, 3)
	.word 0x118 1 											# set b(2, 0)
	.word 0x119 2 						 					# set b(2, 1)
	.word 0x11A 3 											# set b(2, 2)
	.word 0x11B 4 						 					# set b(2, 3)
	.word 0x11C 1 											# set b(3, 0)
	.word 0x11D 2 						 					# set b(3, 1)
	.word 0x11E 3 											# set b(3, 2)
	.word 0x11F 4 						 					# set b(3, 3)

main:
	add $sp, $zero, $imm2, $zero, 0, 2000					# set stack at 2000
	add $sp, $sp, $imm2, $zero, 0, -5						# adjust stack for 5 items
	sw $zero, $sp, $imm2, $s0, 0, 4							# save $s0
	sw $zero, $sp, $imm2, $s1, 0, 3							# save $s1
	sw $zero, $sp, $imm2, $s2, 0, 2							# save $s2
	sw $zero, $sp, $imm2, $a0, 0, 1 						# save $a0
	sw $zero, $sp, $imm2, $a1, 0, 0 						# save $a1 
	add $a0, $zero, $imm2, $zero, 0, 0x100					# get first matrix, first row address to a0
	add $a1, $zero, $imm2, $zero, 0, 0x110					# get second matrix, first column address to a1
	add $s0, $zero, $imm2, $zero, 0, 0x120					# result matrix first element address
		
col_loop_a1:
	jal $ra, $zero, $zero, $imm2, 0, mul_vec 				# mul_vec
	sw $v0, $s0, $zero, $zero, 0, 0 						# save v0 to res_mat[$s0]
	add $s0, $s0, $imm2, $zero, 0, 1 						# $s0++
	add $a1, $a1, $imm2, $zero, 0, 1 						# $a1++
	add $t0, $a0, $a1, $zero, 0, 0 							# $t0 = $a1 + $a0 
	bgt $zero, $a1, $imm1, $imm2, 0x113, row_loop_a0 		# if a1 > 0x113: row_loop_a0
	beq $zero, $zero, $zero, $imm2, 0, col_loop_a1 			# else col_loop_a1
	
row_loop_a0:
	add $a1, $zero, $imm2, $zero, 0, 0x110 					# $a1 = 0x110
	add $a0, $a0, $imm2, $zero, 0, 4 						# $a0 += 4
	bgt $zero, $a0, $imm1, $imm2, 0x10C, END 				# if a0 > 0x10C: END
	beq $zero, $zero, $zero, $imm2, 0, col_loop_a1 			# else col_loop_a1


mult_vec:
	add $sp, $sp, $imm2, $zero, 0, -2						# adjust stack for 2 items
	sw $zero, $sp, $imm2, $a0, 0, 1 						# save $a0
	sw $zero, $sp, $imm2, $a1, 0, 0 						# save $a1
	add $t0, $zero, $zero, $zero, 0, 0 						# set $t0 (counter) to 0
	add $v0, $zero, $zero, $zero, 0, 0						# set $v0 = 0
inner_loop:
	lw $t1, $a0, $zero, $zero, 0, 0 						# load $t1 = mat0[$a0]
	lw $t2, $a1, $zero, $zero, 0, 0 						# load $t2 = mat1[$a1]
	mac $v0, $t1, $t2, $v0, 0, 0 							# mult and sum to $v0
	add $t0, $t0, $imm2, $zero, 0, 0 						# $t0 (counter) ++
	add $a0, $a0, $imm2, $zero, 0, 1 						# $a0++
	add $a1, $a1, $imm2, $zero, 0, 4 						# $a1 += 4
	blt $zero, $t0, $imm1, $imm2, 4, inner_loop 			# if $t0 (counter) < 4: inner_loop
	lw $a1, $sp, $imm2, $zero, 0, 0 						# restore $a1
	lw $a0, $sp, $imm2, $zero, 0, 1 						# restore $a0
	add $sp, $sp, $imm2, $zero, 0, 2						# adjust stack pointer by 2
	beq $zero, $zero, $zero, $ra, 0, 0 						# return to $ra
		
END:
	lw $a1, $sp, $imm2, $zero, 0, 0 						# restore $a1
	lw $a0, $sp, $imm2, $zero, 0, 1 						# restore $a0
	lw $s2, $sp, $imm2, $zero, 0, 2							# restore $s2
	lw $s1, $sp, $imm2, $zero, 0, 3							# restore $s1
	lw $s0, $sp, $imm2, $zero, 0, 4							# restore $s0
	add $sp, $sp, $imm2, $zero, 0, 3						# adjust stack pointer
	halt $zero, $zero, $zero, $zero, 0, 0					# halt
	
	
	
	