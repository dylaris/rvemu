.option norvc
.option nopic

# @ident: name=amomaxu.w, opcode=0x2f, funct3=0x2, funct5high=0x1c
# @expect: rd=1, rs1=2, rs2=3
amomaxu.w x1, x3, (x2)
