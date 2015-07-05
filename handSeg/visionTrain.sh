#!/usr/bin/expect
cd /Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg
spawn scp output_noise1.mpeg output_noise2.mpeg output_noise3.mpeg output_noise4.mpeg jw859@amdpool.ece.cornell.edu:./
expect "wujilongece2013!!!"
send "wujilongece2013!!!\r"
interact

spawn ssh jw859@amdpool.ece.cornell.edu  /home/student/jw859/CVProject/EvaluationPara
expect "wujilongece2013!!!"
send "wujilongece2013!!!\r"
interact


