.option norvc
.option nopic

# @ident: name=blt, opcode=0b1100011, funct3=0x4

# @expect: rd=0, rs1=1, rs2=2, imm=12
blt x1, x2, 3f
1:
# @expect: rd=0, rs1=2, rs2=3, imm=4
blt x2, x3, 2f
2:
# @expect: rd=0, rs1=3, rs2=4, imm=0
blt x3, x4, 2b
3:
# @expect: rd=0, rs1=4, rs2=5, imm=-4
blt x4, x5, 2b
4:
