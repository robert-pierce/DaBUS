        addi    $a1,    $zero,  42           # $a1 = 42
        addi    $a2,    $zero,  255          # $a2 = 255
        nand    $a1,    $a1,    $a2          # $a1 = -43
        add     $a0,    $a1,    $a2          # $a0 = -1
        sw      $a0,    0x2a($zero)          # MEM[2a] = -1
        lw      $a1,    0x2a($zero)          # $a1 = -1
        ei
        di
        
