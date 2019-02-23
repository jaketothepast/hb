# HB

The Host Blocker command.

This is a system utility written in C that allows one to quickly black hole hosts in the /etc/hosts file, keeping one from resolving any DNS for those hosts and permanently blocking them on the system.

# Motivations

I would like to get better with C, and will be using C much more in the near future. In addition, I have issues with browsing to sites that hurt my productivity and am always on the command line.

# TODO

1. Get  delete working for my /etc/hosts editor.
2. Daemonize this program to be started up by systemd.
    - As part of daemonization, remember and remove added hosts on teardown.
3. Log dns requests that look at the hosts file -- To see how many go where