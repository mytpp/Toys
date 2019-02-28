# Perosonal Network Storage

PNS is originally a course work for Network Storage Technologies. It's a simple distributed filesystem connected by network and aimed to let one or more persons share a common view into the filesystem.


## Paths
There're three kinds of path in the system.   
**Logical path** is the main interface provided for user, and is shared by the whole filesystem. It starts with a single '/', like '/movies/2018/avengers.mp4'.    
**Physical path** starts with '//', like '//hostName/somewhere/in/specified/root' or '//ip:port/somewhere/in/specified/root', the latter is mainly used as a middle format for communication.  
**Local path** specify a position in local host, like 'D:\Games\DarkSouls' 

The relation between Logical paths and Physical paths is maintained by tracker. User use the `ln` command to link a local path (recursively) to logical path(see below).   



## Usage
Each node(computer) in the network should run a PNS daemon:
```
python ./pns.py -m daemon -c *.yml
```
`-m` means 'mode', `-c` means 'config'.  
The first node started must be a tracker ('istracker: true' in config file), who keeps the meta database about which node each path is on.

To manipulate the filesystem, run PNS in shell mode:
```
python ./pns.py -m shell -c *.yml ls //
```
The command `ls //` lists all the hosts (nodes) in this system. It can be replaced by other command listed below. 
Altogether, six commands are supported:
* ln src dst
* ls dst
* md dir
* rm dst
* cp src dst
* mv src dst

## communication protocol

Field |Explanation |Request Required|Response Required| Valid value
------|------------|--------|--------|-----------------
V     |Version     |Yes     |        |'h1 1.0' or 'h2 HB' ('HB' for heartbeat)
A     |Authentication |Yes  |        |SHA256( secret + C )
C     |Command     |Yes     |        |
E     |Error Code  |        | Yes    |same as HTTP error code
L     |Length      |        |        |# of content's bytes in decimal format
|     |Content     |        |        |

Fields in header are divided by '\n'. Header and content are divided by an extra empty line (ie. another '\n').
