.option norvc
.option nopic
# @ident: name=fclass.s, opcode=0x53, funct3=0x1, funct5low=0x00, funct7=0x70
# @expect: rd=0, rs1=1
fclass.s x0, f1
