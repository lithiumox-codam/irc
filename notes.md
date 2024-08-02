# Notes

## Available clients at CODAM
On computers at CODAM we have both nc (netcat) and irssi available for use. Both act a bit different so we have to be careful.

## Non blocking IO
Most non-blocking frameworks use an infinite loop that constantly checks (polls) if data is returned from IO. This is often called the event loop. An event loop is literally a while(true) loop that in each iteration will check if data is ready to read from a network socket. Technically, sockets are implemented as file descriptors (FD) on UNIX systems. It is therefore better to say that a FD is checked for ready data. The list of FDs that you want to check for ready data is generally called the interest list.

### ”event”, “FD readiness” and “FD is ready for data” are synonyms in this text

## Use epoll() instead of poll()
epoll() blocks if there is no ready data in the FDs of the interest list. This means that there are no unnecessary iterations in the event loop when there is no ready FD data.

When there is new data available in at least one of the FDs in the interest list, and when this data is not (completely) read/emptied from the FDs, then in the next iteration of the event loop this FD will not be seen as having new data ready. This will save unnecessary iterations in the case that a FD is not completely read when the next iteration of the event loop is executed.

[Link to article](https://medium.com/ing-blog/how-does-non-blocking-io-work-under-the-hood-6299d2953c74)

## Useful links

https://github.com/marineks/Ft_irc
https://ircgod.com/

## Example event loop
event loop:
	accept connections, save them in a list
	for each connection in the list:
		poll for incoming data
		if data is available:
			read data
			if data not complete:
				continue
			else:
				process data (parse):
				write message to all clients
