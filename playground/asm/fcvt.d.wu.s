.option norvc
.option nopic
# @ident: name=fcvt.d.wu, opcode=0x53, funct3=0x0, funct5low=0x01, funct7=0x69
# @expect: rd=0, rs1=1
fcvt.d.wu f0, x1
