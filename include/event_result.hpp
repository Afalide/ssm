
#pragma once

#define TINYSM_RESULT_BASIC_EVENT_DONE     1 // no additional actions required for this event
#define TINYSM_RESULT_TRANSIT_EVENT_DONE   4 // no additional actions required for this transition
#define TINYSM_RESULT_FORWARD_EVENT        5 // forward this event to the sub-state

//#define TINYSM_RESULT_REQ_PROCESSOR_START  2 // a stopped processor cannot restart (this involves processing an event requesting to start)
//#define TINYSM_RESULT_REQ_PROCESSOR_STOP   3
