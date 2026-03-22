.option norvc
.option nopic

# @ident: name=beq, opcode=0b1100011, funct3=0x0

# @expect: rd=0, rs1=1, rs2=2, imm=20
beq x1, x2, 5f
1:
# @expect: rd=0, rs1=2, rs2=3, imm=8
beq x2, x3, 3f
2:
# @expect: rd=0, rs1=3, rs2=4, imm=0
beq x3, x4, 2b
3:
# @expect: rd=0, rs1=4, rs2=5, imm=4
beq x4, x5, 4f
4:
# @expect: rd=0, rs1=5, rs2=6, imm=-4
beq x5, x6, 3b
5:
