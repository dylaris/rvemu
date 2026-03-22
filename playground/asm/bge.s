.option norvc
.option nopic

# @ident: name=bge, opcode=0b1100011, funct3=0x5

# @expect: rd=0, rs1=1, rs2=2, imm=4
bge x1, x2, 1f
1:
# @expect: rd=0, rs1=2, rs2=3, imm=12
bge x2, x3, 4f
2:
# @expect: rd=0, rs1=3, rs2=4, imm=-4
bge x3, x4, 1b
3:
# @expect: rd=0, rs1=4, rs2=5, imm=0
bge x4, x5, 3b
4:
