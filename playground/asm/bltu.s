.option norvc
.option nopic

# @ident: name=bltu, opcode=0b1100011, funct3=0x6

# @expect: rd=0, rs1=1, rs2=2, imm=4
bltu x1, x2, 1f
1:
# @expect: rd=0, rs1=2, rs2=3, imm=4
bltu x2, x3, 2f
2:
# @expect: rd=0, rs1=3, rs2=4, imm=4
bltu x3, x4, 3f
3:
