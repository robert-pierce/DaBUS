main:

             # Book-Keeping
                addi,    $sp,    $sp,   -13;            # move stack pointer 
                sw,      $ra,    0($sp);                # save return address 
                sw,      $fp,    1($sp);                # save old frame pointer
                addi,    $fp,    $sp,   13;             # set up frame pointer

                sw,      $zero,  0($fp);                # save ODDS = 0 on stack  
                sw,      $zero,  -1($fp);               # save EVENS = 0 on stack  
                sw,      $zero,  -2($fp);               # save TOTAL = 0 on stack 

                addi,    $t0,    $zero,  7;
                sw,      $t0,    -10($fp);              # save a[0] = 7 on stack                        inst: 10

                addi,    $t0,    $zero,  5;
                sw,      $t0,    -9($fp);               # save a[1] = 5 on stack

                addi,    $t0,    $zero,  6;
                sw,      $t0,    -8($fp);               # save a[2] = 6 on stack  

                addi,    $t0,    $zero,  3;
                sw,      $t0,   -7($fp);                # save a[3] = 3 on stack

                addi,    $t0,    $zero,  2;
                sw,      $t0,    -6($fp);               # save a[4] = 2 on stack  

                addi,    $t0,    $zero,  5;
                sw,      $t0,    -5($fp);               # save a[5] = 5 on stack                        inst:20

                addi,    $t0,    $zero,  8;
                sw,      $t0,    -4($fp);               # save a[6] = 8 on stack  

                addi,    $t0,    $zero,  6;
                sw,      $t0,    -3($fp);               # save a[7] = 6 on stack
        
                addi,    $t0,    $zero,  8;
                sw,      $t0,    -11($fp);              # save sizea = 8 on stack
             # END book-keeping
       
                add,     $s0,    $zero,  $zero;         # init i = 0
                addi,    $s1,    $zero,  13;            # set $s1 to 13 
                
Loop_A:
                beq,     $s0,    $s1,    15;            # check if i < 13, if no jump to Loop_B         inst: 29

             # Call fact(i)
                addi,     $a0,    $s0,   0;             # put arg i in $a0  # fact(i) routine call      inst: 30
                jal,      0x3c;                         # call fact(i) # return from fact(i) routine call
             # RETURN from fact()
        
                add,     $t0,    $2,     $zero;         # move return value of fact to $t0
                addi,    $t1,    $zero,   1;            # set $t1 = 1
                and,     $t0,     $t0,    $t1;          # is x (mod 2) == 0?
                beq,     $t0,    $zero,  4;

                lw,      $t0,    0($fp);                # load odds from stack
                addi,    $t0,    $t0,    1;             # increment odds
                sw,      $t0,    0($fp);                # store odds back to stack
                j,       0x2a;                          # jump to End_Loop_A
        
Evens:
                lw,      $t0,    -1($fp);               # load evens from stack                         inst: 40
                addi,    $t0,    $t0,    1;             # increment evens
                sw,      $t0,    -1($fp);               # store evens back to stack
               
End_Loop_A:     
                addi,    $s0,    $s0,    1;             # i++                                           inst: 43
                j,       0x1c;                          # jump to beginning of Loop_A

Init_Loop_B:    
                addi,    $t0,    $zero,  0;             # init j = 0                                    inst: 45
                lw,      $t1,    -11($fp);              # load sizea into $t1
                addi,    $t2,    $zero,  0;             # init $t2 = 0
Loop_B:
                beq,     $t0,    $t1,    6;             # check if j < sizea, if not exit loop          inst: 48
                addi,    $t4,    $fp,    -10;           # load a[0] address into $t4
                add,     $t4,    $t4,    $t0;           # set $t4 to address a[j]                       inst: 50
                lw,      $t3,    0($t4);                # load a[j] from .data
                add,     $t2,    $t2,    $t3;           # total += a[j]
        
                addi,     $t0,    $t0,    1;             # j++
                j,       0x2f;                          # jump back to beginning of LoopB
        
Exit_Loop_B:    
                sw,      $t2,    -2($fp);               # store total on stack                          inst: 55

             # Book-Keeping  
                lw,      $ra,    -13($fp);              # restore return address
                lw,      $fp,    -12($fp);              # restore old frame pointer
                addi,    $sp,    $sp,    13;            # pop stack frame
                addi,    $t9,    $t9,     0;             # dummy operation, b/c I don't want to adjust addresses  inst: 59
                halt;
             # END PROGRAM

Fact:
             # Book-Keeping
                addi,    $sp,    $sp,    -3 ;           # stack frame is 2 words deep                   inst: 61 
                sw,      $ra,    0($sp);                # save return address
                sw,      $fp,    1($sp);                # save frame pointer
                addi,    $fp,    $sp,    2;             # set up frame pointer
                sw,      $a0,    0($fp);                # save argument (n)
             # END book-keeping
        
                add,     $t0,    $zero,  $zero;         # set $t0 = 0
                beq,     $a0,    $t0,    6;             # if ($a0 == 0) jump Return_1                    inst: 67 
            
             # Recursive call fact(n - 1);
                lw,      $v1,    0($fp);                # load n
                addi,    $a0,    $v1,    -1;            # compute n - 1,  save as arg for recursive fact() call
                jal,     0x3c;                          # call fact(n-1)                                 inst: 70
             # Return from fact(n-1)
        
                lw,      $v1,    0($fp);                # load n                                         
                mul,     $v0,    $v0,    $v1;           # compute n * fact(n - 1)
                j,       0x4a;                          # jump to Return                                 inst: 73
        
Return_1:       
                addi,    $v0,    $zero,  1;             # load 1 into return value                       inst: 74      

Return:
        # book-keeping
                lw,      $ra,    -2($fp);               # restore $ra                                    inst: 75
                lw,      $fp,    -1($fp);               # restore $fp
                addi,    $sp,    $sp,    3;             # pop stack
                jr,      $ra;                           # return to caller                               inst: 78
