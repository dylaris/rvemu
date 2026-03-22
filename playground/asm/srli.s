.option norvc
.option nopic

# @ident: name=srli, opcode=0b0010011, funct3=0x5, funct7=0x00

# @expect: rd=1, rs1=2, imm=0
srli x1, x2, 0
# @expect: rd=2, rs1=3, imm=1
srli x2, x3, 1
# @expect: rd=3, rs1=4, imm=7
srli x3, x4, 7
# @expect: rd=4, rs1=5, imm=15
srli x4, x5, 15
# @expect: rd=5, rs1=6, imm=31
srli x5, x6, 31
