 main:
	add $sp, $sp, $imm2, $zero, 0, -4				# adjust stack for 4 items
	sw $zero, $sp, $imm2, $s0, 0, 3					# save $s0
	sw $zero, $sp, $imm2, $s1, 0, 2					# save $s1
	sw $zero, $sp, $imm2, $s2, 0, 1					# save $s2
	sw $zero, $sp, $immw, $a0 0, 0 					# save $a0
	lw $a0, $imm1, $zero, $zero, 0x100, 0 			# load R
	mac $a0, $a0, $a0, $zero, 0, 0 					# $a0 = R^2
	out $zero, $zero, $imm1, $imm2, 21, 255 		# monitor data(IO21) = 255
	add $t0, $zero, $zero, $zero, 0, 0 				# x_counter = 0
	add $t1, $zero, $zero, $zero, 0, 0 				# y_counter = 0

outer_loop:
	jal $ra, $zero, $zero, $imm2, 0, inner_loop 	# jump to inner loop
	add $t1, $t1, $imm1, $zero, 1, 0 				# y++
	add $t0, $zero, $zero, $zero, 0, 0 				# x_counter = 0
	beq $zero, $zero, $zero, $imm2, 0, outer_loop 	# jump to outer_loop
	
inner_loop:
	sub $t2, $t1, $imm1, $zero, 128, 0 				# $t2 = y - y_center
	mac $t2, $t2, $t2, $zero, 0, 0 					# $t2 = (y - y_center)^2
	sub $s0, $t0, $imm1, $zero, 128, 0 				# $s0 = x - x_center
	mac $s0, $s0, $s0, $zero, 0, 0 					# $s0 = (x - x-center)^2
	add $t2, $t2, $s0, $zero, 0, 0 					# $t2 = (y - y_center)^2 + (x - x-center)^2
	ble $zero, $t2, $a0, $imm1, plot, 0 			# if x-center^2 + y-center^2 <= R^2: plot
	add $t0, $t0, $imm1, $zero, 1, 0 				# x++
	add $s1, $t0, $t1, $zero, 0, 0 					# $s1 = x_counter + y_counter
	beq $zero, $s1, $imm1, $imm2, 511, END 			# if x_counter == 256 and y_counter == 255 jump to END
	beq $zero, $t0, $imm1, $ra, 256, 0 				# else if x_counter == 256 return to the line y++ in outer_loop
	beq $zero, $zero, $zero, $imm1, inner_loop, 0 	# else return to inner_loop
	
plot:
	mac $s2, $t1, $imm1, $t0, 256, 0 				# $s2 = 256*y_counter + x_counter (pixel address)
	out $zero, $imm1, $zero, $s2, 20, 0 			# give pixel address to monitor address register(20)
	out $zero, $imm1, $zero, $imm2, 22, 1 			# set monitorcmd register(22) to 1
	out $zero, $imm1, $zero, $imm2, 22, 0 			# set monitorcmd register(22) to 0
	beq $zero, $zero, $zero, $imm1, inner_loop, 0 	# return to inner loop
	
END:
	lw $s0, $sp, $imm2, $zero, 0, 0					# restore $a0
	lw $s2, $sp, $imm2, $zero, 0, 1					# restore $s2
	lw $s1, $sp, $imm2, $zero, 0, 2					# restore $s1
	lw $s0, $sp, $imm2, $zero, 0, 3					# restore $s0
	add $sp, $sp, $imm2, $zero, 0, 4				# adjust stack pointer
	halt $zero, $zero, $zero, $zero, 0, 0			# halt