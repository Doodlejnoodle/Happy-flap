# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)       #save register onto stack
	#addi	sp,sp,-4     # dec stack ppinter by 4 bytes, downward
	#sw	%reg,0(sp) 
#.end_macro

#.macro	POP (%reg)
	#lw	%reg,0(sp) #store value 
	#addi	sp,sp,4     #move the stack pointer bakup
#.end_macro
###################
# Macros for DTEK #
###################
.macro	PUSH reg
	addi sp,sp,-4
	sw \reg,0(sp) 
.endm

.macro	POP reg
	lw	\reg,0(sp)
	addi	sp,sp,4
.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x5957  #16 bit rep of time
timstr:	.asciz 	"text more text lots of text\0"
	.text
	.globl timetemplate, tick, time2string, delay, display_string 

# Function for displaying a string with a newline at the end	

display_string:	
	li a7,4     # a7 holds system call number which is a0
	ecall      # prints string a7 = 4
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr    #load adress of string
	jal     display_string #print the string
	
	# wait a little
	li	a0, 1000	#prepare delay 1000ms, a0 = ms
	jal	delay            #busy wait for 1 sec
	
	# call tick
	la 	a0, mytime     #update time for 1 sec
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)    #load mytime vale in a1
	jal	time2string
	
	# go back and do it all again
	j	timetemplate   #repeat indefinitly

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

#########################################################
# Place for your functions: time2string, hex2asc, delay.#
#########################################################

#time2string
# a0 = dest buffer
# a1= time_info (nibblar: [M10] [M1] [S10] [S1])
# output: "MM:SS\0"

        
        
time2string:

         PUSH ra   #store return address
         addi t0, a0, 0    #t temp regis for cal and a argu to return values
         
 
         
         #minute tens
         srli  t3, a1, 12    #shifting 4 bytes towad right, get minute tens digit
         andi t3, t3, 0xF  #mask lower nibble (o-15)
         addi a0, t3, 0    #Mtens strings in a0
         jal ra,  hexasc    # convert to ASCII
         sb   a0, 0(t0)      #store in string buffer
        
         
         
         #minute ones
         srli t3, a1, 8
         andi t3, t3, 0xF
         addi a0, t3, 0
         jal  ra, hexasc
         sb a0, 1(t0)
        
         
         #colon 
         addi  a0, zero,  0x3A
         sb a0, 2(t0)    # add at index 2
         
         
         #second tens
         srli t3, a1, 4
         andi t3, t3, 0xF
         addi a0, t3, 0
         jal  ra, hexasc
         sb a0, 3(t0)
         
         
         # second ones
         srli t3, a1, 0 
         andi t3, t3, 0xF
         addi a0, t3, 0
       
         jal ra, hexasc 
         sb a0, 4(t0)
         
         
        #addi a0, ZERO, 2
         
         
         #Null
         addi a0, zero, 0x00
         sb  zero, 5(t0)
        
        
    POP ra  #restore return addres 
    jr ra     #return to caller
    
    
    #Delay function for this program to waste time
         
         
 delay:     #simple busy loop to waste time
 while:       #outer for mili sec and inner for lop fo cpu delays for som cycles
        ble a0, x0, return   #if ms = a0 <= 0 then break it or return  
         addi a0, a0, -1      #which means ms = ms -1
         
        li t0, 1570      #loop limit
        addi t1, x0, 0    # i = o
   
 for:
       addi t1, t1, 1       # i = i+1 /i++
        blt t1, t0, for    # while i < 4711
        
         j while      #repeat while loop               
        
 return:
          jr ra      #return    
                      
        

      #Hexasc function from previous assigmnet 
      
  hexasc:
        andi a0, a0, 0xF  #talet mellan 0-15, längsta 4 bitarna
        li t1, 10               #t1 gränsen between 0-9 & a-f
        blt a0, t1, digit           #if a0 is less than 10
        
        addi a0, a0, -10
        addi a0, a0, 'A' 
        jr ra
        
            
 digit:
        
        addi a0, a0, '0'  #offset foo and add ao 
        jr ra   
      
      
 #delay:
       #jr ra
                   
                           
                                   
                                           
                                                           
                                                                                   
                                                                                                                                                       
                                                                                                                                                                                                                                                                                             
       
         

        
     
        
