.option norvc
.option nopic

# @ident: name=jalr, opcode=0x67, funct3=0x0
# @expect: rd=1, rs1=2, imm=0
jalr x1, x2, 0
# @expect: rd=2, rs1=3, imm=4
jalr x2, x3, 4
# @expect: rd=3, rs1=4, imm=-8
jalr x3, x4, -8
