.option norvc
.option nopic

# @ident: name=sltiu, opcode=0b0010011, funct3=0x3

# @expect: rd=1, rs1=2, imm=0
sltiu x1, x2, 0
# @expect: rd=2, rs1=3, imm=1
sltiu x2, x3, 1
# @expect: rd=3, rs1=4, imm=2047
sltiu x3, x4, 2047
# @expect: rd=4, rs1=5, imm=100
sltiu x4, x5, 100
