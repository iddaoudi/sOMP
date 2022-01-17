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
#        / /         | |         \ \                   / /         |          \ \
#   l0up/ /l0dn  l1up| |l1dn  l2up\ \l2dn         l3up/ /l3dn  l4up|l4dn   l5up\ \l5dn
#      / /           | |           \ \               / /           |            \ \
#     host0         host1         host2           host3         host4          host5

numa = sys.argv[1]
numa = int(numa)

ncpu = sys.argv[2]
ncpu = int(ncpu)

def prettify(elem):
    # Return a pretty-printed XML string for the Element.
    rough_string = ElementTree.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

comment = Comment('!DOCTYPE platform SYSTEM "https://simgrid.org/simgrid.dtd"')
platform = Element('platform', version='4.1')
socket0 = SubElement(platform, 'zone', id='SOCKET-0', routing='Full')
numa = [SubElement(socket0, 'zone', id='NUMA-'+str(i), routing='Cluster') for i in range(numa)]
numa_counter = 0
ncpu_counter = 0
for z in zone:
    router = SubElement(z, 'router', id='router-'+str(numa_counter))
    for i in range(ncpu):
        host = SubElement(z, 'host', id='CPU-'+str(i + ncpu_counter), speed='100Gf')
        link = SubElement(z, 'link', id='link-'+str(i + ncpu_counter), bandwidth='500GBps', latency='0ns', sharing_policy='SPLITDUPLEX')
        host_link = SubElement(z, 'host_link', id='CPU-'+str(i + ncpu_counter), up='link-'+str(i + ncpu_counter)+'_UP', down='link-'+str(i + ncpu_counter)+'_DOWN')
    numa_counter += 1
    ncpu_counter += ncpu
connector = [SubElement(socket, 'link', id='link-'+str(i)+'-'+str(j), bandwidth='500GBps', latency='0ns') for i in range(numa-1) for j in range(i+1, numa)]
zoneroute = [SubElement(socket, 'zoneRoute', src='NUMA-'+str(i), dst='NUMA-'+str(j), gw_src='router-'+str(i), gw_dst='router-'+str(j)) for i in range(numa-1) for j in range(i+1, numa)]
numa_counter1 = 0
numa_counter2 = 1
for r in zoneroute:
    link_ctn = SubElement(r, 'link_ctn', id='link-'+str(numa_counter1)+'-'+str(numa_counter2))
    numa_counter2 += 1
    if (numa_counter2 == numa):
        numa_counter1 += 1
        numa_counter2 = numa_counter1 + 1

print(prettify(platform))
