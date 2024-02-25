 main:
	lw $a0, $imm1, $zero, $zero, 0x100, 0 		# load R
	add $t0, $zero, $zero, $zero, 0, 0 			# x = 0
	add $t1, $a0, $zero, $zero, 0, 0 			# y = R
	add $t2, $a0, $imm1, $zero, -1, 0 			# p = 1 - R
	add $s0, $zero, $imm1, $zero, 128, 0 		# x_center = 128
	add $s1, $zero, $imm1, $zero, 257, 0 		# y_center = 257
	mac $s1, $s1, $imm1, $zero, 128, 0 			# y_center = (256 + 1)128, should be the center pixel

	
Loop:
	bge $zero, $t0, $t1, $imm1, END, 0 			# if x >= y, END	
	beq $zero, $zero, $zero, $imm1, plot, 0 	# plot
	add $t0, $t0, $imm1, $zero, 1, 0 			# x++
	bgt, $zero, $t2, $zero, $imm1, inc_p, 0 	# if p < 0
	add $t1, $t1, $imm1, $zero, -1, 0 			# y -= 1
	sub $s2, $t0, $t1, $zero, 0, 0 				# $s2 = (x-y)
	mac $s2, $s2, $imm1, $imm2, 2, 1 			# $s2 = 2(x-y) + 1
	add $t2, $t2, $s2, $zero, 0, 0 				# p += 2(x-y) + 1
	
inc_p:
	add $s2, $t0, $t0, $imm1, 1, 0 				# $s2 = 2x + 1
	add $t2, $t2, $s2, $zero, 0, 0				# p += 2x + 1
	beq $zero, $zero, $zero, $imm1, Loop, 0 	# jump to loop
	
plot:
	# actual plot
	beq $zero, $zero, $zero, $imm1, Loop, 0 	# jump to loop
	
END:
