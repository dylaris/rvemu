.option rvc
.option nopic

# @ident: name=c.fsd, copcode=0x0, cfunct3=0x5
# @expect: rd=0, rs1=8, rs2=9, imm=0
c.fsd f9, 0(x8)

