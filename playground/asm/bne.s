.option norvc
.option nopic

# @ident: name=bne, opcode=0b1100011, funct3=0x1

# @expect: rd=0, rs1=1, rs2=2, imm=4
bne x1, x2, 1f
1:
# @expect: rd=0, rs1=2, rs2=3, imm=4
bne x2, x3, 2f
2:
# @expect: rd=0, rs1=3, rs2=4, imm=4
bne x3, x4, 3f
3:
# @expect: rd=0, rs1=4, rs2=5, imm=4
bne x4, x5, 4f
4:
# @expect: rd=0, rs1=5, rs2=6, imm=4
bne x5, x6, 5f
5:
