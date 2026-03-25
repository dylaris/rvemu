.option norvc
.option nopic

# @ident: name=ld, opcode=0x03, funct3=0x3

# @expect: rd=1, rs1=2, imm=0
ld x1, 0(x2)

# @expect: rd=3, rs1=4, imm=8
ld x3, 8(x4)

# @expect: rd=5, rs1=6, imm=-16
ld x5, -16(x6)

