.option norvc
.option nopic

# @ident: name=bgeu, opcode=0b1100011, funct3=0x7

# @expect: rd=0, rs1=1, rs2=2, imm=4
bgeu x1, x2, 1f
1:
# @expect: rd=0, rs1=2, rs2=3, imm=8
bgeu x2, x3, 3f
2:
# @expect: rd=0, rs1=3, rs2=4, imm=-4
bgeu x3, x4, 1b
3:
