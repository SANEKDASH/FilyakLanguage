jmp Р°РіР°РЅРёРј
аганим:
		;VAR 'коэф_а' DECLARATION
 ;ASSIGN
	in
	pop [rbx+1]
		;VAR 'коэф_б' DECLARATION
 ;ASSIGN
	in
	pop [rbx+2]
		;VAR 'коэф_ц' DECLARATION
 ;ASSIGN
	in
	pop [rbx+3]
;IF
	push 0
	push 0
;PUSH VAR 'коэф_а'
	push [rbx+1]
	push 0
	sub
	je equal_op_end_0
	push 0
	jmp logical_op_end_0
equal_op_end_0:
	push 1
logical_op_end_0:
	jbe end_if_0
;FUNC CALL
;PUSH VAR 'коэф_б'
	push [rbx+2]
	pop [rbx+5]
;PUSH VAR 'коэф_ц'
	push [rbx+3]
	pop [rbx+6]
	push rbx
	push rbx
	push 4
	add
	pop rbx
	call решить_линейный_случай
	pop rbx
	push [rbx+4]			;ret_value
;ABORT
	hlt

end_if_0:
cycle_start_0:
	push 0
	push 1
	jbe while_end_1
;PRINT
	push 228
	out
	jmp cycle_start_0
cycle_end_0:
;FUNC CALL
;PUSH VAR 'коэф_а'
	push [rbx+1]
	pop [rbx+5]
;PUSH VAR 'коэф_б'
	push [rbx+2]
	pop [rbx+6]
;PUSH VAR 'коэф_ц'
	push [rbx+3]
	pop [rbx+7]
	push rbx
	push rbx
	push 4
	add
	pop rbx
	call решить_квадратный_случай
	pop rbx
	push [rbx+4]			;ret_value
hlt
дискриминант_считай:
;RETURN
;SUB
;MULT
;PUSH VAR 'коэф_б'
	push [rbx+2]
;PUSH VAR 'коэф_б'
	push [rbx+2]
	mult
;MULT
;MULT
	push 4
;PUSH VAR 'коэф_а'
	push [rbx+1]
	mult
;PUSH VAR 'коэф_ц'
	push [rbx+3]
	mult
	sub
	pop [rbx]
ret
ret
решить_линейный_случай:
;IF
	push 0
	push 0
;PUSH VAR 'коэф_б'
	push [rbx+1]
	push 0
	sub
	je equal_op_end_1
	push 0
	jmp logical_op_end_1
equal_op_end_1:
	push 1
logical_op_end_1:
	jbe end_if_2
;PRINT
	push 0
	out
;RETURN
	push 0
	pop [rbx]
ret
end_if_2:
		;VAR 'корень' DECLARATION
 ;ASSIGN
;DIV
;MULT
	push -1
;PUSH VAR 'коэф_ц'
	push [rbx+2]
	mult
;PUSH VAR 'коэф_б'
	push [rbx+1]
	div
	pop [rbx+3]
;PRINT
	push 1
	out
;PRINT
;PUSH VAR 'корень'
	push [rbx+3]
	out
;RETURN
	push 1
	pop [rbx]
ret
ret
решить_квадратный_случай:
		;VAR 'дискриминант' DECLARATION
 ;ASSIGN
;FUNC CALL
;PUSH VAR 'коэф_а'
	push [rbx+1]
	pop [rbx+8]
;PUSH VAR 'коэф_б'
	push [rbx+2]
	pop [rbx+9]
;PUSH VAR 'коэф_ц'
	push [rbx+3]
	pop [rbx+10]
	push rbx
	push rbx
	push 7
	add
	pop rbx
	call дискриминант_считай
	pop rbx
	push [rbx+7]			;ret_value
	pop [rbx+4]
;IF
	push 0
	push 0
;PUSH VAR 'дискриминант'
	push [rbx+4]
	push 0
	sub
	jb less_op_end_0
	push 0
	jmp logical_op_end_2
less_op_end_0:
	push 1
logical_op_end_2:
	jbe end_if_4
;PRINT
	push 0
	out
;RETURN
	push 0
	pop [rbx]
ret
end_if_4:
		;VAR 'корень_1' DECLARATION
 ;ASSIGN
;DIV
;SUB
;MULT
	push -1
;PUSH VAR 'коэф_б'
	push [rbx+2]
	mult
;SQRT
;PUSH VAR 'дискриминант'
	push [rbx+4]
	sqrt
	sub
;MULT
	push 2
;PUSH VAR 'коэф_а'
	push [rbx+1]
	mult
	div
	pop [rbx+5]
		;VAR 'корень_2' DECLARATION
 ;ASSIGN
;DIV
;ADD
;MULT
	push -1
;PUSH VAR 'коэф_б'
	push [rbx+2]
	mult
;SQRT
;PUSH VAR 'дискриминант'
	push [rbx+4]
	sqrt
	add

;MULT
	push 2
;PUSH VAR 'коэф_а'
	push [rbx+1]
	mult
	div
	pop [rbx+6]
;PRINT
	push 2
	out
;PRINT
;PUSH VAR 'корень_1'
	push [rbx+5]
	out
;PRINT
;PUSH VAR 'корень_2'
	push [rbx+6]
	out
;RETURN
	push 2
	pop [rbx]
ret
ret
