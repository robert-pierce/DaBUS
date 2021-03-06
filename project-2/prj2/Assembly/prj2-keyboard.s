! NAME: Robert Pierce
! GTLogin:rpierce31
!=================================================================
! General conventions:
!   1) Stack grows from high addresses to low addresses, and the
!      top of the stack points to valid data
!
!   2) Register usage is as implied by assembler names and manual
!
!   3) Function Calling Convention:
!
!       Setup)
!       * Immediately upon entering a function, push the RA on the stack.
!       * Next, push all the registers used by the function on the stack.
!
!       Teardown)
!       * Load the return value in $v0.
!       * Pop any saved registers from the stack back into the registers.
!       * Pop the RA back into $ra.
!       * Return by executing jalr $ra, $zero.
!=================================================================

!vector table
vector0:    .fill 0x00000000 !0
            .fill 0x00000000 !1
            .fill 0x00000000 !2
            .fill 0x00000000
            .fill 0x00000000 !4
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000 !8
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000
            .fill 0x00000000 !15
!end vector table

main:           la      $sp, stack           ! Initialize stack pointer
                lw      $sp, 0($sp)          
                
                ! Install timer interrupt handler into vector table              
                la      $a0, ti_inthandler   ! get address of interrupt handler for timer
                sw      $a0, 1($zero)        ! load address of interrupt handler into 0x00000001

                la      $a0, keyboard_handler ! get address of interrupt handler for keyboard
                sw      $a0, 2($zero)        ! load address of keyboard handler into 0z00000002
        
                ei                           ! Don't forget to enable interrupts...

                la      $at, factorial       ! load address of factorial label into $at
                addi    $a0, $zero, 5        ! $a0 = 5, the number to factorialize
                jalr    $at, $ra             ! jump to factorial, set $ra to return addr
                halt                         ! when we return, just halt

factorial:      addi    $sp, $sp, -1         ! push RA
                sw      $ra, 0($sp)
                addi    $sp, $sp, -1         ! push a0
                sw      $a0, 0($sp)
                addi    $sp, $sp, -1         ! push s0
                sw      $s0, 0($sp)
                addi    $sp, $sp, -1         ! push s1
                sw      $s1, 0($sp)

                beq     $a0, $zero, base_zero
                addi    $s1, $zero, 1
                beq     $a0, $s1, base_one
                beq     $zero, $zero, recurse
                
    base_zero:  addi    $v0, $zero, 1   ! 0! = 1
                beq     $zero, $zero, done

    base_one:   addi    $v0, $zero, 1   ! 1! = 1
                beq     $zero, $zero, done

    recurse:    add     $s1, $a0, $zero      ! save n in s1
                addi    $a0, $a0, -1         ! n! = n * (n-1)!
                la      $at, factorial
                jalr    $at, $ra

                add     $s0, $v0, $zero      ! use s0 to store (n-1)!
                add     $v0, $zero, $zero    ! use v0 as sum register
    mul:        beq     $s1, $zero, done     ! use s1 as counter (from n to 0)
                add     $v0, $v0, $s0
                addi    $s1, $s1, -1
                beq     $zero, $zero, mul

    done:       lw      $s1, 0($sp)          ! pop s1
                addi    $sp, $sp, 1
                lw      $s0, 0($sp)          ! pop s0
                addi    $sp, $sp, 1
                lw      $a0, 0($sp)          ! pop a0
                addi    $sp, $sp, 1
                lw      $ra, 0($sp)          ! pop RA
                addi    $sp, $sp, 1
                jalr    $ra, $zero

 ti_inthandler: addi    $sp, $sp, -1         ! push $k0 onto stack
                sw      $k0,    0($sp)

                ei                           ! enable interrupts
        
                addi    $sp, $sp, -1         ! push $ra
                sw      $ra, 0($sp)
                addi    $sp, $sp, -1         ! push $a0
                sw      $a0, 0($sp)
                addi    $sp, $sp, -1         ! push $s0
                sw      $s0, 0($sp)
                addi    $sp, $sp, -1         ! push $s1
                sw      $s1, 0($sp)
                addi    $sp, $sp, -1         ! push $v0
                sw      $v0, 0($sp)
                addi    $sp, $sp, -1         ! push $at
                sw      $at, 0($sp)


!!! interrupt code
                la      $a0, seconds          ! load seconds label
                lw      $a0, 0($a0)           ! get address of seconds in MEM
                lw      $a1, 0($a0)           ! load MEM[seconds]
        
                addi    $a3, $zero, 59        ! load 59 into $a3

                beq     $a1, $a3, inc_minutes !is seconds = 59?  
                addi    $a1, $a1, 1          ! inc_seconds
                sw      $a1, 0($a0)          ! store seconds in MEM
                beq     $zero, $zero,  clean_up

   inc_minutes: add     $a1, $zero, $zero    ! reset seconds to zero
                sw      $a1, 0($a0)          ! store reset seconds in MEM

                la      $a0, minutes         ! load minutes label
                lw      $a0, 0($a0)          ! get address of minutes in MEM
                lw      $a1, 0($a0)          ! load MEM[minutes]
        
                beq     $a1, $a3,  inc_hours ! is minutes = 59?
                addi    $a1, $a1,  1         ! inc minutes
                sw      $a1, 0($a0)          ! store minutes in MEM
                beq     $zero, $zero, clean_up 

    inc_hours:  add     $a1, $zero, $zero    ! reset minutes
                sw      $a1, 0($a0)          ! store reset minutes in MEM

                la      $a0, hours           ! load hours label
                lw      $a0, 0($a0)          ! get address of hours in MEM
                lw      $a1, 0($a0)          ! load hours
        
                addi    $a1, $a1,  1         ! inc hours

                sw      $a1, 0($a0)          ! store hours in MEM

    clean_up:   lw      $at, 0($sp)          ! pop $at from the stack
                addi    $sp, $sp, 1    
                lw      $v0, 0($sp)          ! pop $v0
                addi    $sp, $sp, 1      
                lw      $s1, 0($sp)          ! pop $s1
                addi    $sp, $sp, 1    
                lw      $s0, 0($sp)          ! pop $s0
                addi    $sp, $sp, 1
                lw      $a0, 0($sp)          ! pop $a0
                addi    $sp, $sp, 1    
                lw      $ra, 0($sp)          ! pop $ra
                addi    $sp, $sp, 1

                di                           ! disable interrupts
        
                lw      $k0, 0($sp)          ! pop $k0
                addi    $sp, $sp, 1

                reti                         ! reurn from interrupt

        
 keyboard_handler: addi    $sp, $sp, -1      ! push $k0 onto stack
                sw      $k0,    0($sp)

                addi    $sp, $sp, -1         ! push $a0
                sw      $a0, 0($sp)

                bonj    $a0, $zero           ! writes the keyboard char out to $a0

         !! need to read keyboard before interupts enabled
         !! b/c a nested keyboard interrupt will clobber the
         !! old keyboard char value stored in $a0

                ei                           ! enable interrupts
        
                addi    $sp, $sp, -1         ! push $ra
                sw      $ra, 0($sp)
                addi    $sp, $sp, -1         ! push $s0
                sw      $s0, 0($sp)
                addi    $sp, $sp, -1         ! push $s1
                sw      $s1, 0($sp)
                addi    $sp, $sp, -1         ! push $v0
                sw      $v0, 0($sp)
                addi    $sp, $sp, -1         ! push $at
                sw      $at, 0($sp)      


        !! interrupt code
                

                la      $a1, keyboard_out    ! get address keyboard_out label
                lw      $a2, 0($a1)          ! load the memory address to store key out

                sw      $a0, 0($a2)          ! store key out in memory

                addi    $a2, $a2, 1          ! increment key out memory address

                sw      $a2,  0($a1)         ! store incremented key out address in keyboard_out label


        !! clean stack
                lw      $at, 0($sp)          ! pop $at from the stack
                addi    $sp, $sp, 1    
                lw      $v0, 0($sp)          ! pop $v0
                addi    $sp, $sp, 1      
                lw      $s1, 0($sp)          ! pop $s1
                addi    $sp, $sp, 1    
                lw      $s0, 0($sp)          ! pop $s0
                addi    $sp, $sp, 1
                lw      $ra, 0($sp)          ! pop $ra
                addi    $sp, $sp, 1
                lw      $a0, 0($sp)          ! pop $a0
                addi    $sp, $sp, 1    

                di                           ! disable interrupts
        
                lw      $k0, 0($sp)          ! pop $k0
                addi    $sp, $sp, 1

                reti                         ! reurn from interrupt



stack:        .fill 0xA00000
seconds:      .fill 0xFFFFFC
minutes:      .fill 0xFFFFFD
hours:        .fill 0xFFFFFE
keyboard_out: .fill 0xEEEEE0
