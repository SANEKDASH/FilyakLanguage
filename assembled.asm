jmp аганим
������:
		;VAR '����_�' DECLARATION
 ;ASSIGN
	in
	pop [rbx+1]
		;VAR '����_�' DECLARATION
 ;ASSIGN
	in
	pop [rbx+2]
		;VAR '����_�' DECLARATION
 ;ASSIGN
	in
	pop [rbx+3]
;IF
	push 0
	push 0
;PUSH VAR '����_�'
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
;PUSH VAR '����_�'
	push [rbx+2]
	pop [rbx+5]
;PUSH VAR '����_�'
	push [rbx+3]
	pop [rbx+6]
	push rbx
	push rbx
	push 4
	add
	pop rbx
	call ������_��������_������
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
;PUSH VAR '����_�'
	push [rbx+1]
	pop [rbx+5]
;PUSH VAR '����_�'
	push [rbx+2]
	pop [rbx+6]
;PUSH VAR '����_�'
	push [rbx+3]
	pop [rbx+7]
	push rbx
	push rbx
	push 4
	add
	pop rbx
	call ������_����������_������
	pop rbx
	push [rbx+4]			;ret_value
hlt
������������_������:
;RETURN
;SUB
;MULT
;PUSH VAR '����_�'
	push [rbx+2]
;PUSH VAR '����_�'
	push [rbx+2]
	mult
;MULT
;MULT
	push 4
;PUSH VAR '����_�'
	push [rbx+1]
	mult
;PUSH VAR '����_�'
	push [rbx+3]
	mult
	sub
	pop [rbx]
ret
ret
������_��������_������:
;IF
	push 0
	push 0
;PUSH VAR '����_�'
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
		;VAR '������' DECLARATION
 ;ASSIGN
;DIV
;MULT
	push -1
;PUSH VAR '����_�'
	push [rbx+2]
	mult
;PUSH VAR '����_�'
	push [rbx+1]
	div
	pop [rbx+3]
;PRINT
	push 1
	out
;PRINT
;PUSH VAR '������'
	push [rbx+3]
	out
;RETURN
	push 1
	pop [rbx]
ret
ret
������_����������_������:
		;VAR '������������' DECLARATION
 ;ASSIGN
;FUNC CALL
;PUSH VAR '����_�'
	push [rbx+1]
	pop [rbx+8]
;PUSH VAR '����_�'
	push [rbx+2]
	pop [rbx+9]
;PUSH VAR '����_�'
	push [rbx+3]
	pop [rbx+10]
	push rbx
	push rbx
	push 7
	add
	pop rbx
	call ������������_������
	pop rbx
	push [rbx+7]			;ret_value
	pop [rbx+4]
;IF
	push 0
	push 0
;PUSH VAR '������������'
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
		;VAR '������_1' DECLARATION
 ;ASSIGN
;DIV
;SUB
;MULT
	push -1
;PUSH VAR '����_�'
	push [rbx+2]
	mult
;SQRT
;PUSH VAR '������������'
	push [rbx+4]
	sqrt
	sub
;MULT
	push 2
;PUSH VAR '����_�'
	push [rbx+1]
	mult
	div
	pop [rbx+5]
		;VAR '������_2' DECLARATION
 ;ASSIGN
;DIV
;ADD
;MULT
	push -1
;PUSH VAR '����_�'
	push [rbx+2]
	mult
;SQRT
;PUSH VAR '������������'
	push [rbx+4]
	sqrt
	add

;MULT
	push 2
;PUSH VAR '����_�'
	push [rbx+1]
	mult
	div
	pop [rbx+6]
;PRINT
	push 2
	out
;PRINT
;PUSH VAR '������_1'
	push [rbx+5]
	out
;PRINT
;PUSH VAR '������_2'
	push [rbx+6]
	out
;RETURN
	push 2
	pop [rbx]
ret
ret
