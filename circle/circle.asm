 main:
	lw $a0, $imm1, $zero, $zero, 0x100, 0 			# load R
	mac $a0, $a0, $a0, $zero, 0, 0 					# $a0 = R^2
	out $zero, $zero, $imm1, $imm2, 21, 255 		# monitor data(IO21) = 255
	add $t0, $zero, $zero, $zero, 0, 0 				# x_counter = 0
	add $t1, $zero, $zero, $zero, 0, 0 				# y_counter = 0


outer_loop:
	beq $zero, $zero, $zero, $imm2, 0, inner_loop 	# jump to inner loop
	add $t1, $t1, $imm1, $zero, 1, 0 				# y++
	add $t0, $zero, $zero, $zero, 0, 0 				# x_counter = 0
	beq $zero, $zero, $zero, $imm2, 0, outer_loop 	# jump to outer_loop
	
inner_loop:
	sub $t2, $t1, $imm1, $zero, 128, 0 				# y - y_center
	mac $t2, $t2, $t2, $zero, 0, 0 					# $t2 = (y - y_center)^2
	sub $s0, $t0, $imm1, $zero, 128, 0 				# $s0 = x - x-center
	mac $s0, $s0, $s0, $zero, 0, 0 					# $s0 = (x - x-center)^2
	add $t2, $t2, $s0, $zero, 0, 0 					# $t2 = (y - y_center)^2 + (x - x-center)^2
	ble $zero, $t2, $a0, $imm1, plot, 0 			# if x-center^2 + y-center^2 <= R^2: plot
	add $t0, $t0, $imm1, $zero, 1, 0 				# x++
	# if x_counter == 255 and y_counter == 255 END
	beq $zero, $t0, $imm1, outer_loop, 255, 0 		# else if x_counter == 255 outer loop
	beq $zero, $zero, $zero, inner_loop, 0, 0 		# else return to inner_loop
	
plot:
	# actual plot
	beq $zero, $zero, $zero, inner_loop, 0, 0 		# return to inner loop
	
END:
	
