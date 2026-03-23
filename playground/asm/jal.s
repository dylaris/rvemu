.option norvc
.option nopic

# @ident: name=jal, opcode=0x6f
# @expect: rd=1, imm=16
jal x1, 4f
1:
# @expect: rd=2, imm=0
jal x2, 1b
2:
# @expect: rd=3, imm=4
jal x3, 3f
3:
# @expect: rd=4, imm=-4
jal x4, 2b
4:
