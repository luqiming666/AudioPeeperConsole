# AudioPeeperConsole
使用Windows Core Audio APIs的loopback模式截获声卡数据，然后... 想干嘛干嘛~

本Demo中，将截获的音频数据保存为.pcm文件，然后可以使用ffmpeg进行测试播放：
<br>
ffplay.exe -ar 48000 -ac 2 -f f32le -i d:\peep.pcm
