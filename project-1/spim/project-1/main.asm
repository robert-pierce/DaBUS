        .text
        .globl main
main:
           # book-keeping
                addi    $sp,    $sp,    -32             # move stack pointer
                sw      $ra,    20($sp)                 # save return address
                sw      $fp,    16($sp)                 # save old frame pointer
                addi    $fp,    $sp,    28              # set up frame pointer
           # END book-keeping
        
                add     $s0,    $zero,  $zero           # init i = 0
                addi    $s1,    $zero,  52              # set $s1 to 13 (52 = 13*40)
                
Loop_A:
                beq     $s0,    $s1,   Init_Loop_B      # check if i < 13, if no jump to Loop_B

           # fact(i) routine call
                srl     $a0,    $s0,    2               # put arg i in $a0
                jal     fact                            # call fact(i)
           # return from fact(i) routine call
        
                add     $t0,    $v0,    $zero           # move return value of fact to $t0
                addi    $t1,    $zero   1               # set $t1 = 1
                and     $t0     $t0,    $t1             # is x (mod 2) == 0?
                beq     $t0,    $zero,  Evens

                lw      $t0,    odds($zero)             # load odds from .data
                addi    $t0,    $t0,    1               # increment odds
                sw      $t0,    odds($zero)             # store odds back to .data
                j       End_Loop_A                      # jump to End_Loop_A
        
Evens:
                lw      $t0,    evens($zero)            # load evens from .data
                addi    $t0,    $t0,    1               # increment evens
                sw      $t0,    evens($zero)            # store evens back to .data
               
End_Loop_A:     
                addi    $s0,    $s0,    4               # i++        
                j       Loop_A                          # jump to beginning of Loop_A

Init_Loop_B:    
                add     $t0,    $zero,  $zero           # init j = 0
                lw      $t1,    sizea($zero)            # load sizea into $t1
                sll     $t1,    $t1,    2               # multiply index by 4
                add     $t2,    $zero,  $zero           # init $t2 = 0
Loop_B:
                beq     $t0,    $t1,    Exit_Loop_B     # check if j < sizea, is not exit loop
                lw      $t3,    a($t0)                  # load a[j] from .data
                add     $t2,    $t2,    $t3             # total += a[j]
        
                add     $t0,    $t0,    4               # j++
                j       Loop_B                          # jump back to beginning of LoopB
        
Exit_Loop_B:    
                sw      $t2,    total($zero)            # store total in .data

           # book-keeping
                lw      $ra,    20($sp)                 # restore return address
                lw      $fp,    16($sp)                 # restore frame pointer
                addi    $sp,    $sp,    32              # pop stack frame
                j       $ra                             # return to caller


        .data
odds:   .word   0
evens:  .word   0
total:  .word   0
a:      .word   7, 5, 6, 3, 2, 5, 8, 6
sizea:  .word   8


