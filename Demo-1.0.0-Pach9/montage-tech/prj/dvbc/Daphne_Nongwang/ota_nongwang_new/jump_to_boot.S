/*
 * jump_to_boot.S
 *
 *  Created on: 2012-2-21
 *      Author: grli
 */


.extern test_print
.export  __restore_enviroment_b
.align 4
__restore_enviroment_b:

psrclr ie, fe
psrset ee

mov r0, r2
ldw 	r1,(r0,0) 			// Restore PSR
mtcr	r1,PSR



addi	r0,4
ldm r1-r15,(r0)
addi	r0,28 				// Increment SP past the registers
addi	r0,32 				// Increment SP past the registers

//lrw 	r6,test_p
//stw 	r15,(r6,0)

//mov r10, r15


//mov r15, r10
jmp r15

