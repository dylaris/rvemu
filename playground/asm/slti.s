.option norvc
.option nopic

# @ident: name=slti, opcode=0b0010011, funct3=0x2

# @expect: rd=1, rs1=2, imm=0
slti x1, x2, 0
# @expect: rd=2, rs1=3, imm=-1
slti x2, x3, -1
# @expect: rd=3, rs1=4, imm=1
slti x3, x4, 1
# @expect: rd=4, rs1=5, imm=2047
slti x4, x5, 2047
# @expect: rd=5, rs1=6, imm=-2048
slti x5, x6, -2048
