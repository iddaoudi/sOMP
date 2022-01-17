from xml.etree.ElementTree import Element, tostring, SubElement, Comment
from xml.etree import ElementTree
from xml.dom import minidom
import sys

# This script describes a NUMA architecture for SimGrid(S4U) / SOMP APIs.
# Example of what this program can produce from an architecture POV:
#
#                 router0 - - - - - - - link0-1 - - - - - - - - router1
#        __________numa0___________                   __________numa1___________
#        |                        |                   |                        |
#        |        backbone0       |                   |      backbone1         |
#        |________________________|                   |________________________|
#        / /         | |         \ \                   / /        | |          \ \
#   l0up/ /l0dn  l1up| |l1dn  l2up\ \l2dn         l3up/ /l3dn l4up| |l4dn   l5up\ \l5dn
#      / /           | |           \ \               / /          | |            \ \
#     host0         host1         host2           host3          host4          host5

numa = sys.argv[1]
numa = int(numa)

npack = sys.argv[2]
npack = int(npack)

ncpu = sys.argv[3]
ncpu = int(ncpu)

ncache = sys.argv[4]
ncache = int(ncache)

cachesize = sys.argv[5]
cachesize = int(cachesize)

alpha = sys.argv[6]

beta = sys.argv[7]

counter = 0
counter_bis = 0
ncpu_counter = 0
numa_counter = 0

def prettify(elem):
    # Return a pretty-printed XML string for the Element.
    rough_string = ElementTree.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

platform = Element('platform', version='4.1')
comment = Comment('!DOCTYPE platform SYSTEM "https://simgrid.org/simgrid.dtd"')
socket = SubElement(platform, 'zone', id='SOCKET-0', routing='Full')
pack = [SubElement(socket, 'zone', id='PACK-'+str(i), routing='Full') for i in range(npack)]

for p in pack:
    prop1 = SubElement(p, 'prop', id='ncache', value=str(ncache))
    prop2 = SubElement(p, 'prop', id='cachesize', value=str(cachesize))
    prop3 = SubElement(p, 'prop', id='alpha', value=str(alpha))
    prop4 = SubElement(p, 'prop', id='beta', value=str(beta))
    nnuma = [SubElement(p, 'zone', id='NUMA-'+str(i+numa_counter), routing='Full') for i in range(numa)]
    numa_counter += numa
    for n in nnuma:
        router   = SubElement(n, 'router', id='router-'+str(counter))
        backbone = SubElement(n, 'backbone', id='router-'+str(counter), bandwidth='500GBps', latency='0ns')
        for i in range(ncpu):
            host  = SubElement(n, 'host', id='CPU-'+str(i + ncpu_counter), speed='100Gf')
            link  = SubElement(n, 'link', id='link-'+str(i + ncpu_counter), bandwidth='500GBps', latency='0ns', sharing_policy='SPLITDUPLEX')
            host_link = SubElement(n, 'host_link', id='CPU-'+str(i + ncpu_counter), up='link-'+str(i + ncpu_counter)+'_UP', down='link-'+str(i + ncpu_counter)+'_DOWN')
        counter      += 1
        ncpu_counter += ncpu
    numalink  = [SubElement(p, 'link', id='link-'+str(100 - counter_bis - i), bandwidth='500GBps', latency='0ns') for i in range(numa)]
    zoneroute = [SubElement(p, 'zoneRoute', src='NUMA-?', dst='NUMA-?', gw_src='router-?', gw_dst='router-?') for i in range(numa)]
    for z in zoneroute:
        ctn       = SubElement(z, 'link_ctn', id='link-'+str(100 - counter_bis))
    counter_bis += 1

biglink = SubElement(socket, 'link', id='link-'+str(100 - counter_bis - numa - 1), bandwidth='500GBps', latency='0ns')
bigzoneroute = SubElement(socket, 'zoneRoute', src='NUMA-?', dst='NUMA-?', gw_src='router-?', gw_dst='router-?')
bigctn = SubElement(bigzoneroute, 'link_ctn', id='link-'+str(100 - counter_bis))


print(prettify(platform))
