#ifndef _SWITCH_H_
#define _SWITCH_H_

//! Possible commands for the switch
enum class ESwitchCommand
{
    None,   //!< No action
    On,     //!< Turn switch ON
    Off,    //!< Turn switch OFF
    Toggle, //!< Toggle current state
    Pulse,  //!< Toggle state, wait, toggle back
};

//! Performs initial configuration of the switch
void ConfigureSwitch();

//! Performs the requested switch command
void OperateSwitch(ESwitchCommand);

#endif