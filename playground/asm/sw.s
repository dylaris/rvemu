.option norvc
.option nopic

# @ident: name=sw, opcode=0b0100011, funct3=0x2

# @expect: rd=0, rs1=2, rs2=1, imm=0
sw x1, 0(x2)
# @expect: rd=0, rs1=3, rs2=2, imm=4
sw x2, 4(x3)
# @expect: rd=0, rs1=4, rs2=3, imm=-4
sw x3, -4(x4)
# @expect: rd=0, rs1=5, rs2=4, imm=100
sw x4, 100(x5)
# @expect: rd=0, rs1=6, rs2=5, imm=-200
sw x5, -200(x6)
# @expect: rd=0, rs1=7, rs2=6, imm=2044
sw x6, 2044(x7)
# @expect: rd=0, rs1=8, rs2=7, imm=-2048
sw x7, -2048(x8)
