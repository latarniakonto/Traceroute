## Basic Traceroute implementation

## Official mannual description
*Traceroute tracks the route packets taken from an IP network on their way to a given host.<br>
It utilizes the IP protocol's time to live (TTL) field and attempts to elicit an ICMP TIME_EXCEEDED response from each
gateway along the path to the host.*

## Running the program (Linux)
1. Navigate to the main program folder aka */Traceroute* <br>
2. Run *make* command. It may require installing some additional software.
3. Run *./traceroute <IPV4_ADDR>* command with administrative privileges. Only administrator can open raw sockets in Linux operating system.

## Basic Traceroute implementation

## Official mannual description
*Traceroute tracks the route packets taken from an IP network on their way to a given host.<br>
It utilizes the IP protocol's time to live (TTL) field and attempts to elicit an ICMP TIME_EXCEEDED response from each
gateway along the path to the host.*

## Running the program (Linux)
1. Navigate to the main program folder aka */Traceroute* <br>
2. Run *make* command. It may require installing some additional software.
3. Run *./traceroute <IPV4_ADDR>* command with administrative privileges. Only administrator can open raw sockets in Linux operating system.

## Output
```
1. x.x.x.x 3ms
2. x.x.x.x 24ms
3. *
4. *
5. *
6. *
7. *
8. 8.8.8.8 33ms
```

