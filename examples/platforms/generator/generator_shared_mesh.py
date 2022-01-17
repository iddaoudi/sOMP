from xml.etree.ElementTree import Element, tostring, SubElement, Comment
from xml.etree import ElementTree
from xml.dom import minidom
import sys
import math

# This script describes a NUMA architecture for SimGrid(S4U) / SOMP APIs.
# Example of what this program can produce from an architecture POV:
#
#                 router0 - - - - - - - link0-1 - - - - - - - - router1
#        __________numa0___________                   __________numa1___________
#        |                        |                   |                        |
#        |                        |                   |                        |
#        |________________________|                   |________________________|
#        / /         | |         \ \                   / /         |          \ \
        #   l0up/ /l0dn  l1up| |l1dn  l2up\ \l2dn         l3up/ /l3dn  l4up|l4dn   l5up\ \l5dn
#      / /           | |           \ \               / /           |            \ \
        #     host0         host1         host2           host3         host4          host5

numa = sys.argv[1]
numa = int(numa)

ncpu = sys.argv[2]
ncpu = int(ncpu)

nlink = sys.argv[3]
nlink = int(nlink)

def prettify(elem):
    # Return a pretty-printed XML string for the Element.
    rough_string = ElementTree.tostring(elem, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    return reparsed.toprettyxml(indent="  ")

comment = Comment('!DOCTYPE platform SYSTEM "https://simgrid.org/simgrid.dtd"')
platform = Element('platform', version='4.1')
socket = SubElement(platform, 'zone', id='SOCKET-0', routing='Full')
numa = [SubElement(socket, 'zone', id='NUMA-'+str(i), routing='Floyd') for i in range(numa)]
numa_counter = 0
ncpu_counter = 0
nlink_counter = 0
nlink_counter2 = 0
j = 0
for z in numa:
    router = SubElement(z, 'router', id='router-'+str(numa_counter))
    for i in range(ncpu):
        host = SubElement(z, 'host', id='CPU-'+str(i + ncpu_counter), speed='100Gf')
    router_link = SubElement(z, 'link', id='linkr-'+str(numa_counter), bandwidth='500GBps', latency='0ns', sharing_policy='SHARED')
    for i in range(nlink):
        link = SubElement(z, 'link', id='link-'+str(i + nlink_counter), bandwidth='500GBps', latency='0ns', sharing_policy='SHARED')
    route_router = SubElement(z, 'route', src='router-'+str(numa_counter), dst='CPU-'+str(ncpu_counter))
    ctn_router = SubElement(route_router, 'link_ctn', id='linkr-'+str(numa_counter))
    for i in range(ncpu):
        if ((i + ncpu_counter + 1) % math.sqrt(ncpu) == 0):
            continue
        route = SubElement(z, 'route', src='CPU-'+str(i + ncpu_counter), dst='CPU-'+str(i + ncpu_counter + 1))
        ctn = SubElement(route, 'link_ctn', id='link-'+(str(j)))
        j += 1
    for i in range(ncpu - int(math.sqrt(ncpu))):
        route = SubElement(z, 'route', src='CPU-'+str(i + ncpu_counter), dst='CPU-'+str(i + int(math.sqrt(ncpu)) + ncpu_counter))
        ctn2 = SubElement(route, 'link_ctn', id='link-'+(str(j)))
        j += 1
    numa_counter += 1
    ncpu_counter += ncpu
    nlink_counter += nlink

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
