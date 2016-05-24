        .text
        .globl fact

fact:
           # book-keeping
                addi    $sp,    $sp,    -32             # stack frame is 32 bytes long
                sw      $ra,    20($sp)                 # save return address
                sw      $fp,    16($sp)                 # save frame pointer
                addi    $fp,    $sp,    28              # set up frame pointer
                sw      $a0,    0($fp)                  # save argument (n)
           # END book-keeping
        
                add     $t0,    $zero,  $zero           # set $t0 = 0
                beq     $a0,    $t0,    Return_1        # if ($a0 == 0) jump 
            
           # make routine call fact($a0 - 1)
                lw      $v1,    0($fp)                  # load n
                addi    $a0,    $v1,   -1               # compute n - 1,  save as arg for recursive fact() call
                jal     fact                            # call fac(n-1)

                lw      $v1,    0($fp)                  # load n
                mul     $v0,   $v0,    $v1              # compute n * fact(n - 1)
                j       Return
        
Return_1:       
                addi    $v0,    $zero,  1               # load 1 into return value

Return:
        # book-keeping
                lw      $ra,    20($sp)                 # restore $ra
                lw      $fp,    16($sp)                 # restore $fp
                addi    $sp,    $sp,    32              # pop stack
                j       $ra                             # return to caller
