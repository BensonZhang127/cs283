1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines when a command's output is fully received from the server by receiving an EOF (End Of File) character, which is defined as RDSH_EOF_CHAR (0x04). This character is sent by the server after the output of a command is finished, signaling to the client that the command execution has completed. The client can use a buffer to receive data until EOF is detected which help deal with partial read and ensure complete transmissions.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

A networked shell protocol should implement message framing to handle TCP's stream-oriented nature, where each message includes a header containing the length and type of the following data. This header allows the receiver to know exactly how much data to expect and when a complete command or response has been received. Without proper framing, critical issues can arise such as commands running together, partial command execution, buffer overflows, and the inability to match responses with their corresponding commands. Challenges that may arise if not handled correctly is that the data could be read incorrectly, excessive memory usage, and client stalling waiting for data, which will likely cause more issues making the protocol not secure and not reliable.

3. Describe the general differences between stateful and stateless protocols.

The general difference between stateful and stateless protocols are like their name implies, the stateful protocol keep track of the state of the connection, session, or previous interactions with clients. Meanwhile, stateless protocols don’t and are independent, so each request must contain all information necessary to fulfill the request. 

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

In lecture, we did say that UDP is “unreliable”, but it is still reliable enough to be good. The comparison made was with mailing systems since even though not ALL mails are always successfully delivered, it is consistent and reliable enough to be a decent system. The tradeoff was essentially performance versus reliability, and UDP with less overhead than TCP and faster transmission can be used for video streaming or online gaming where occasional packet loss can be tolerated. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

In lecture, we did say that UDP is “unreliable”, but it is still reliable enough to be good. The comparison made was with mailing systems since even though not ALL mails are always successfully delivered, it is consistent and reliable enough to be a decent system. The tradeoff was essentially performance versus reliability, and UDP with less overhead than TCP and faster transmission can be used for video streaming or online gaming where occasional packet loss can be tolerated. 
