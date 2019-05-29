#ifndef fraggerH
#define fraggerH

void ShowHelp();
void HandleSwitches(int argc, char *argv []);
int Fragmentieren();
int DeFragmentieren();
int DoCountDefrags();

#define Version    "0.0.1"

#define NICHTS     0x00
#define FRAGMENT   0x01
#define DEFRAGMENT 0x02

#endif

