.option norvc
.option nopic

# @ident: name=mulh, opcode=0x33, funct3=0x1, funct7=0x01
# @expect: rd=1, rs1=2, rs2=3
mulh x1, x2, x3
# @expect: rd=5, rs1=6, rs2=7
mulh x5, x6, x7
