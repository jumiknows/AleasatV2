;===============================================================================
; Assembly Utility Functions
;===============================================================================

    .text
    .arm

;-------------------------------------------------------------------------------
; Jump to Address
;-------------------------------------------------------------------------------

        .def asm_jump_to_addr
        .asmfunc
asm_jump_to_addr
        mov pc, r0
        .endasmfunc

;-------------------------------------------------------------------------------
; Busy Wait
;-------------------------------------------------------------------------------

        .def asm_busy_wait
        .asmfunc
asm_busy_wait
        subs r0, r0, #1       ; decrement argument by 1
        bmi end_asm_busy_wait ; skip to end if result is negative
        bne asm_busy_wait     ; loop if not at zero yet
end_asm_busy_wait
        bx lr
        .endasmfunc
