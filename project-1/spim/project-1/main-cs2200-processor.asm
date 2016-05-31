        .text
        .globl main
main:
           # book-keeping
                addi    $29,    $29,    -13             # move stack pointer
                sw      $31,    0($29)                  # save return address
                sw      $30,    1($29)                  # save old frame pointer
                addi    $30,    $29,     13             # set up frame pointer

                sw      $0,  0($30)                     # save ODDS = 0 on stack  
                sw      $0,  -1($30)                    # save EVENS = 0 on stack  
                sw      $0,  -2($30)                    # save TOTAL = 0 on stack 

                addi    $8,    $0,      7
                sw      $8,    -10($30)                 # save a[0] = 7 on stack  

                addi    $8,    $0,      5
                sw      $8,    -9($30)                  # save a[1] = 5 on stack

                addi    $8,    $0,      6
                sw      $8,    -8($30)                  # save a[2] = 6 on stack  

                addi    $8,    $0,      3
                sw      $8,    -7($30)                  # save a[3] = 3 on stack

                addi    $8,    $0,      2
                sw      $8,    -6($30)                  # save a[4] = 2 on stack  

                addi    $8,    $0,      5
                sw      $8,    -5($30)                  # save a[5] = 5 on stack

                addi    $8,    $0,      8
                sw      $8,    -4($30)                  # save a[6] = 8 on stack  

                addi    $8,    $0,      6
                sw      $8,    -3($30)                  # save a[7] = 6 on stack
        
                addi    $8,    $0,      8
                sw      $8,    -11($30)                 # save sizea = 8 on stack
        # END book-keeping
        
                add     $16,    $0,     $0              # init i = 0
                addi    $17,    $0,     13              # set $s1 to 13 
                
Loop_A:
                beq     $16,    $17,   Init_Loop_B      # check if i < 13, if no jump to Loop_B

           # fact(i) routine call
                addi     $4,    $16,    0               # put arg i in $a0
                jal     fact                            # call fact(i)
           # return from fact(i) routine call
        
                add     $8,    $2,      $0              # move return value of fact to $t0
                addi    $9,    $0       1               # set $t1 = 1
                and     $8     $8,      $9              # is x (mod 2) == 0?
                beq     $8,    $0,      Evens

                lw      $8,    0($30)                   # load odds from stack
                addi    $8,    $8,      1               # increment odds
                sw      $8,    0($30)                   # store odds back to stack
                j       End_Loop_A                      # jump to End_Loop_A
        
Evens:
                lw      $8,    -1($30)                  # load evens from stack
                addi    $8,    $8,      1               # increment evens
                sw      $8,    -1($30)                  # store evens back to stack
               
End_Loop_A:     
                addi    $16,    $16,    1               # i++        
                j       Loop_A                          # jump to beginning of Loop_A

Init_Loop_B:    
                addi    $8,    $0,      0               # init j = 0
                lw      $9,    -11($30)                 # load sizea into $t1
                addi    $10,    $0,     0               # init $t2 = 0
Loop_B:
                beq     $8,    $9,      Exit_Loop_B     # check if j < sizea, is not exit loop
                addi    $12,    $30,    -10             # load a[0] address into $t4
                add     $12,    $12,    $8              # set $t4 to address a[j]
                lw      $11,    0($12)                  # load a[j] from .data
                add     $10,    $10,    $11             # total += a[j]
        
                add     $8,    $8,      1               # j++
                j       Loop_B                          # jump back to beginning of LoopB
        
Exit_Loop_B:    
                sw      $10,    -2($30)                 # store total on stack

           # book-keeping
                lw      $31,    -13($30)                # restore return address
                lw      $30,    -12($30)                # restore old frame pointer
                addi    $29,    $29,    13              # pop stack frame
                j       $31                             # return to caller
