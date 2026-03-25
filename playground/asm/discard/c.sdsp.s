.option rvc
.option nopic

# @ident: name=c.sdsp, copcode=0x9, cfunct3=0x7
# @expect: rd=0, rs1=2, rs2=1, imm=0
c.sdsp x8, 0(x9)
