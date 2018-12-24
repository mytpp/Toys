# Perosonal Network Storage

PNS is originally a course work for Network Storage Technologies. It's a simple distributed filesystem connected by network and aimed to let one or more persons share a common view into the filesystem.

## Usage
Each node(computer) in the network should run a PNS daemon:
```
python ./pns.py -m daemon -c *.yml
```
The first node started must be a tracker ('istracker: true' in config file), who keeps the meta database about which node each path is on.

To manipulate the filesystem, run PNS in shell mode:
```
python ./pns.py -m shell -c *.yml ls //
```
The command 'ls //' list all the hosts (nodes) in this system.
Altogether, six commands are supported:
* ln src dst
* ls dst
* md dir
* rm dst
* cp src dst
* mv src dst
