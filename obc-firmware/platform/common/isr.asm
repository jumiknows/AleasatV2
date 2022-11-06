;===============================================================================
; Interrupt Service Routines
;
; i.e. The actual functional implementation of the exception handlers
;===============================================================================

    .text
    .arm

;-------------------------------------------------------------------------------
; Imported References
;-------------------------------------------------------------------------------

; portASM.asm
    .ref vPortSWI

; dabort.asm
    .ref _dabort

; sys_phantom.c
    .ref phantomInterrupt

;-------------------------------------------------------------------------------
; Exported References
;-------------------------------------------------------------------------------

    .def isr_undef
    .def isr_swi
    .def isr_pref
    .def isr_data
    .def isr_phantom

;-------------------------------------------------------------------------------
; ISRs
;-------------------------------------------------------------------------------

; Undefined Instruction
isr_undef
        b isr_undef ; TODO not an infinite loop

; Software Interrupt
isr_swi
        b vPortSWI ; TODO avoid extra branch

; Prefetch Abort
isr_pref
        b isr_pref ; TODO not an infinite loop

; Data Abort
isr_data
        b _dabort ; TODO avoid extra branch

; Phantom Interrupt
isr_phantom
        b phantomInterrupt ; TODO avoid extra branch
