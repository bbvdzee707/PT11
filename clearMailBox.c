#include "EV3Mailbox.c"
task main()
{
while(true){
	char msgBufIn[MAX_MSG_LENGTH];  // To contain the incoming message.
	openMailboxIn("EV3_INBOX0");
	waitForMailboxMessage(0);
	readMailboxIn("EV3_INBOX0", msgBufIn);
}
}
