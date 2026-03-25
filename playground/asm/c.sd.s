.option rvc
.option nopic

# @ident: name=c.sd, copcode=0x0, cfunct3=0x7
# @expect: rd=0, rs1=8, rs2=9, imm=0
c.sd x9, 0(x8)

