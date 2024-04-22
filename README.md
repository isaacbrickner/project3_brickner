# Event Buffer Producer-Consumer

## Building

Command line:

* `make` to build. An executable will be produced.
* `make clean` to clean up build products.

## Files

* `pc.c`: The main code to launch the producer-consumer system.
* `eventbuf.c`: Code related to the event buffer.
* `eventbuf.h`: Header file for the event buffer functions.

## Data

The program implements a producer-consumer system using an event buffer. The main data structures is:
* Event Buffer: A buffer to hold events produced by producers and consumed by consumers.

## Functions

* `main()`: Entry point of the program.
  * `producer_thread()`: Function executed by producer threads.
    - Each producer thread generates a sequence of events and adds them to the event buffer.
    - It wait for available spaces in the buffer (`spaces_sem`) and locks the mutex (`mutex_sem`) before adding events.
    - After adding an event, it signals the presence of an item (`items_sem`).
    - It sleeps for a random time before generating the next event.
  * `consumer_thread()`: Function executed by consumer threads.
    - Each consumer thread retrieves events from the event buffer.
    - It waits for available items (`items_sem`) and locks the mutex before retrieving events.
    - If the buffer is empty, it exits the loop.
    - After retrieving an event, it signals the availability of space in the buffer (`spaces_sem`).
    - It sleeps for a random time before retrieving the next event.

## Notes

* Producers generate events and add them to the event buffer, while consumers retrieve events from the buffer.
* The program intendes to demonstrate the synchronization between multiple producers and consumers using semaphores to control thread access.
# project3_brickner
