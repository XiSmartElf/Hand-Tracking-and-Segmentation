#!/usr/bin/expect
cd /Users/BboyKellen/Documents/Xcode/headSeg/handSeg/handSeg
spawn scp output_noiseTest1.mpeg output_noiseTest2.mpeg output_noiseTest3.mpeg output_noiseTest4.mpeg jw859@amdpool.ece.cornell.edu:./
expect "password"
send "password\r"
interact

spawn ssh jw859@amdpool.ece.cornell.edu  /home/student/jw859/CVProject/EvaluationTest
expect "password"
send "password\r"
interact


