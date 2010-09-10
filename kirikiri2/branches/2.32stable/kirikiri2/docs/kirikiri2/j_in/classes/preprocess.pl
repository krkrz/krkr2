# dissolve f_SoundBuffer.xml.in

undef($/);

open FH, "f_SoundBuffer.xml.in";
$content = <FH>;

open FH, ">f_MIDISoundBuffer.xml";
$c = $content;
$c =~ s/<wave>.*?<\/wave>//gs;
$c =~ s/<cdda>.*?<\/cdda>//gs;
$c =~ s/<classname\/>/MIDISoundBuffer/gs;
print FH $c;

open FH, ">f_WaveSoundBuffer.xml";
$c = $content;
$c =~ s/<midi>.*?<\/midi>//gs;
$c =~ s/<cdda>.*?<\/cdda>//gs;
$c =~ s/<classname\/>/WaveSoundBuffer/gs;
print FH $c;

open FH, ">f_CDDASoundBuffer.xml";
$c = $content;
$c =~ s/<wave>.*?<\/wave>//gs;
$c =~ s/<midi>.*?<\/midi>//gs;
$c =~ s/<classname\/>/CDDASoundBuffer/gs;
print FH $c;


