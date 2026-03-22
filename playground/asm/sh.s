.option norvc
.option nopic

# @ident: name=sh, opcode=0b0100011, funct3=0x1

# @expect: rd=0, rs1=2, rs2=1, imm=0
sh x1, 0(x2)
# @expect: rd=0, rs1=3, rs2=2, imm=2
sh x2, 2(x3)
# @expect: rd=0, rs1=4, rs2=3, imm=-2
sh x3, -2(x4)
# @expect: rd=0, rs1=5, rs2=4, imm=32
sh x4, 32(x5)
# @expect: rd=0, rs1=6, rs2=5, imm=-64
sh x5, -64(x6)
# @expect: rd=0, rs1=7, rs2=6, imm=2046
sh x6, 2046(x7)
# @expect: rd=0, rs1=8, rs2=7, imm=-2048
sh x7, -2048(x8)
