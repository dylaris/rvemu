.option rvc
.option nopic

# @ident: name=c.addw, copcode=0x1, cfunct1=0x1, cfunct2high=0x3, cfunct2low=0x1, cfunct3=0x4
# @expect: rd=8, rs1=8, rs2=9
c.addw x8, x9

