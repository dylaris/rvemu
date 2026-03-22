.option norvc
.option nopic

# @ident: name=sb, opcode=0b0100011, funct3=0x0

# @expect: rd=0, rs1=2, rs2=1, imm=0
sb x1, 0(x2)
# @expect: rd=0, rs1=3, rs2=2, imm=1
sb x2, 1(x3)
# @expect: rd=0, rs1=4, rs2=3, imm=-1
sb x3, -1(x4)
# @expect: rd=0, rs1=5, rs2=4, imm=100
sb x4, 100(x5)
# @expect: rd=0, rs1=6, rs2=5, imm=-200
sb x5, -200(x6)
# @expect: rd=0, rs1=7, rs2=6, imm=2047
sb x6, 2047(x7)
# @expect: rd=0, rs1=8, rs2=7, imm=-2048
sb x7, -2048(x8)
