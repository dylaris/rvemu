.option norvc
.option nopic

# @ident: name=lw, opcode=0b0000011, funct3=0x2

# @expect: rd=1, rs1=2, imm=0
lw x1, 0(x2)
# @expect: rd=2, rs1=3, imm=4
lw x2, 4(x3)
# @expect: rd=3, rs1=4, imm=-4
lw x3, -4(x4)
# @expect: rd=4, rs1=5, imm=32
lw x4, 32(x5)
# @expect: rd=5, rs1=6, imm=-64
lw x5, -64(x6)
# @expect: rd=6, rs1=7, imm=2044
lw x6, 2044(x7)
# @expect: rd=7, rs1=8, imm=-2048
lw x7, -2048(x8)
