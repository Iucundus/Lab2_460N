        .ORIG x3000
        AND R0, R0, #0
        ADD R1, R0, #1
        ADD R2, R0, #2
        LEA R3, label
        LDW R3, R3, #0
        HALT
label	.FILL x2120
