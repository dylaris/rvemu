.option norvc
.option nopic

# @ident: name=lhu, opcode=0b0000011, funct3=0x5

# @expect: rd=1, rs1=2, imm=0
lhu x1, 0(x2)
# @expect: rd=2, rs1=3, imm=2
lhu x2, 2(x3)
# @expect: rd=3, rs1=4, imm=-2
lhu x3, -2(x4)
# @expect: rd=4, rs1=5, imm=16
lhu x4, 16(x5)
# @expect: rd=5, rs1=6, imm=-32
lhu x5, -32(x6)
# @expect: rd=6, rs1=7, imm=2046
lhu x6, 2046(x7)
# @expect: rd=7, rs1=8, imm=-2048
lhu x7, -2048(x8)
