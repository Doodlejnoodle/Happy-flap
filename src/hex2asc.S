  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.b

	.text                   #tells the assembler that code follow
	.globl hex2asc         #entry point , main file can call
 
hex2asc:                    #start of function
	li	a0, 17		# + prints the asscii char  load imm 9 into a0, ao use to hold return value
	
	addi    sp,sp,-4    #decrement stack pointer sp by 4 to reserve space
	sw      ra,0(sp)       #save the return address
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	     # write a0 to stdout (single character)
	ecall           # execute the system call (prints the ASCII cha to 1/0 console.

	lw      ra,0(sp)   #load word retsore the saved return adres from the stack
	addi    sp,sp,4      #release thestack space
	jr      ra	 #jump back to caller

  # You can write your own code for hexasc here
  

hexasc:
        andi a0, a0, 0xF  #talet mellan 0-15, längsta 4 bitarna
        li t1, 10               #tem for comparison  
        blt a0, t1, digit           #if a0 is less than 10
        
        addi a0, a0, -10           #if a0 >= 10, then sub 10  (0xA=0)
        addi a0, a0, 'A'     #convert 10-15 to ascii.
        jr ra
        
         
          
 digit:
        
        addi a0, a0, '0'  #if a0 =0then digits
        jr ra       # return to caller / return from subroutine.















     
     
    
