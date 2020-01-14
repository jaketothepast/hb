# HB

The Host Blocker command.

This is a system utility written in C that allows one to quickly black hole hosts in the /etc/hosts file, keeping one from resolving any DNS for those hosts and permanently blocking them on the system.

# How it Works

The program will wake up in a configurable timeframe continuously, reading a config file into memory and making appropriate changes to the /etc/hosts based on what it reads from the config.

# Motivations

I would like to get better with C, and will be using C much more in the near future. In addition, I have issues with browsing to sites that hurt my productivity and am always on the command line.

# TODO

1. Get  delete working for my /etc/hosts editor.
2. Daemonize this program to be started up by systemd.
    - As part of daemonization, remember and remove added hosts on teardown.
    - Can do this with an atexit handler, and a routine in main.
3. Log dns requests that look at the hosts file -- To see how many go where
4. As part of daemonization -- only block hosts within a time limit.
    - Sleep, wakeup, check if time has passed or not, block/unblock host,
      sleep again.
5. Read hosts to block from a config file (to not pollute the /etc/hosts file)
6. Design a unified interface for reading a hosts file. 
