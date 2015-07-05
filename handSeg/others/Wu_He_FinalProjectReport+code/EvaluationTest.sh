export PATH=$PATH:/home/student/jw859/install/stow-pkgs/x86_64-rhel5/bin:/home/student/jw859/install/stow-pkgs/noarch/bin:/research/brg/install/stow-pkgs/x86_64-rhel5/bin:/research/brg/install/stow-pkgs/noarch/bin:/opt/alttools/bin:/opt/synopsys/F-2011.06/bin:/usr/kerberos/bin:/usr/local/bin:/bin:/usr/bin:/classes/ece547/32/v4/bin:/classes/ece547/bin:/opt/sonnet/14.54/bin:/opt/ads/ADS2011_10/bin:/opt/ansoft/hfss15.0/Linux:/opt/ansys/ANSYS-13.0SP2/v130/ansys/bin:/opt/cadence/ic/bin:/opt/cadence/ic/tools/dfII/bin:/opt/cadence/ic/tools/bin:/opt/cadence/ius/tools.lnx86/bin:/opt/cadence/assura/bin:/opt/cadence/ccd/bin:/opt/cadence/confrml/bin:/opt/cadence/cva/bin:/opt/cadence/eanl/bin:/opt/cadence/et/bin:/opt/cadence/ets/bin:/opt/cadence/ext/bin:/opt/cadence/finale/bin:/opt/cadence/icoa/bin:/opt/cadence/ixe/bin:/opt/cadence/mmsim/bin:/opt/cadence/pacific/bin:/opt/cadence/pvs/bin:/opt/cadence/rc/bin:/opt/cadence/ret/bin:/opt/cadence/sev/bin:/opt/cadence/soc/bin:/opt/cadence/spmn/bin:/opt/cadence/tsi/bin:/opt/cadence/xae/bin:/opt/cadence/amskit/tools.lnx86/bin:/opt/cadence/cwspw/tools.lnx86/bin:/opt/cadence/icc/tools.lnx86/bin:/opt/cadence/ifv/tools.lnx86/bin:/opt/cadence/isv/tools.lnx86/bin:/opt/cadence/ius/tools.lnx86/bin:/opt/cadence/kcl/tools.lnx86/bin:/opt/cadence/kmc/tools.lnx86/bin:/opt/cadence/kqv/tools.lnx86/bin:/opt/cadence/lcu/tools.lnx86/bin:/opt/cadence/neocell/tools.lnx86/bin:/opt/cadence/neockt/tools.lnx86/bin:/opt/cadence/psd/tools.lnx86/bin:/opt/cadence/rfkit/tools.lnx86/bin:/opt/cadence/spb/tools.lnx86/bin:/opt/cadence/tda/tools.lnx86/bin:/opt/cadence/tvm/tools.lnx86/bin:/opt/cadence/usim/tools.lnx86/bin:/opt/cadence/vce/tools.lnx86/bin:/opt/cadence/vip/tools.lnx86/bin:/opt/cadence/vsde/tools.lnx86/bin:/opt/comsol/4.2/bin:/classes/brg/install/noarch/bin:/opt/alttools/ece:/classes/ece314/bin:/classes/ece475/bin:/classes/ece547/32/v4/bin:/classes/ece547/bin:/opt/Magic/magic-7.3.108/x86_64/bin:/opt/matlab/R2012a/bin:/opt/QuartusII/10.0SP1/bin:/opt/silvaco/2010-00/bin/:/opt/synplcty/fpga_94/bin:/opt/synplcty/identify_30/bin:/opt/synplcty/system_designer_202/bin
cd /home/student/jw859/CVProject
cp /home/student/jw859/output_noiseTest1.mpeg /home/student/jw859/CVProject/output_noiseTest1.mpeg
cp /home/student/jw859/output_noiseTest2.mpeg /home/student/jw859/CVProject/output_noiseTest2.mpeg
cp /home/student/jw859/output_noiseTest3.mpeg /home/student/jw859/CVProject/output_noiseTest3.mpeg
cp /home/student/jw859/output_noiseTest4.mpeg /home/student/jw859/CVProject/output_noiseTest4.mpeg

vmpegtovx -g f=1,30 if=output_noiseTest1.mpeg of=noise1Out.vx
vclip f=6 if=noise1Out.vx of=6O.vx
vclip f=11 if=noise1Out.vx of=11O.vx
vclip f=16 if=noise1Out.vx of=16O.vx
vclip f=21 if=noise1Out.vx of=21O.vx
vclip f=26 if=noise1Out.vx of=26O.vx
vcat if=6O.vx 11O.vx 16O.vx 21O.vx 26O.vx of=Out.vx
vdim -c if=Out.vx of=Outz.vx 
vrdiff if=InzT1.1.vxa bf=Outz.vx -cp of=compareT1_1.vx ig=InzT1.vx os=compareT1_1.txt



vmpegtovx -g f=1,30 if=output_noiseTest2.mpeg of=noise1Out.vx
vclip f=6 if=noise1Out.vx of=6O.vx
vclip f=11 if=noise1Out.vx of=11O.vx
vclip f=16 if=noise1Out.vx of=16O.vx
vclip f=21 if=noise1Out.vx of=21O.vx
vclip f=26 if=noise1Out.vx of=26O.vx
vcat if=6O.vx 11O.vx 16O.vx 21O.vx 26O.vx of=Out.vx
vdim -c if=Out.vx of=Outz.vx
vrdiff if=InzT2.1.vxa bf=Outz.vx -cp of=compareT2_1.vx ig=InzT2.vx os=compareT2_1.txt



vmpegtovx -g f=1,30 if=output_noiseTest3.mpeg of=noise1Out.vx
vclip f=6 if=noise1Out.vx of=6O.vx
vclip f=11 if=noise1Out.vx of=11O.vx
vclip f=16 if=noise1Out.vx of=16O.vx
vclip f=21 if=noise1Out.vx of=21O.vx
vclip f=26 if=noise1Out.vx of=26O.vx
vcat if=6O.vx 11O.vx 16O.vx 21O.vx 26O.vx of=Out.vx
vdim -c if=Out.vx of=Outz.vx
vrdiff if=InzT3.1.vxa bf=Outz.vx -cp of=compareT3_1.vx ig=InzT3.vx os=compareT3_1.txt




vmpegtovx -g f=1,30 if=output_noiseTest4.mpeg of=noise1Out.vx
vclip f=6 if=noise1Out.vx of=6O.vx
vclip f=11 if=noise1Out.vx of=11O.vx
vclip f=16 if=noise1Out.vx of=16O.vx
vclip f=21 if=noise1Out.vx of=21O.vx
vclip f=26 if=noise1Out.vx of=26O.vx
vcat if=6O.vx 11O.vx 16O.vx 21O.vx 26O.vx of=Out.vx
vdim -c if=Out.vx of=Outz.vx
vrdiff if=InzT4.1.vxa bf=Outz.vx -cp of=compareT4_1.vx ig=InzT4.vx os=compareT4_1.txt


a=`tail -1 compareT1_1.txt | sed 's/dsc//g'`
b=`tail -1 compareT2_1.txt | sed 's/dsc//g'`
c=`tail -1 compareT3_1.txt | sed 's/dsc//g'`
d=`tail -1 compareT4_1.txt | sed 's/dsc//g'`
new=$a$b$c$d
echo $new >> outputT.txt

