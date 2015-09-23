# CPBO Linux

<b>CPBO for Linux, OSX and Windows</b> (without GUI)<br/>
Original CPBO Source: http://www.kegetys.fi/cpbo-source-code-release/<br/>

CPBO extract and pack mission files (*.pbo) for Bohemia Interactive's Armed Assault (ArmA) Series.

Requirement: Boost Libaries, Download via http://www.boost.org/<br/>
Binaries for Windows: http://sourceforge.net/projects/boost/files/boost-binaries/1.58.0/


<b>Usage:</b><br/>
<b>Extract a pbo:</b><br/>
cpbo.exe [-y] -e (filename.pbo) [directory]<br/>
 &gt; directory name is optional, PBO file name used if undefined<br/>
 &gt; optional -y parameter overwrites directory, must be called first!<br/>
<b>Make a pbo:</b><br/>
 cpbo.exe [-y] -p (directory) [filename.pbo]<br/>
 &gt; pbo name is optional, directory name used if undefined<br/>
 &gt; optional -y parameter overwrites pbo, must be called first!<br/>
