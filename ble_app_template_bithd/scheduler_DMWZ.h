#ifndef _scheduler_DMWZ_H__
#define _scheduler_DMWZ_H__

#define SCHED_MAX_EVENT_DATA_SIZE       128//sizeof(app_timer_event_t)                   /**< Maximum size of scheduler events. Note that scheduler BLE stack events do not contain any data, as the events are being pulled from the stack in the event handler. */
#define SCHED_QUEUE_SIZE                20                                          /**< Maximum number of events in the scheduler queue. */

void scheduler_init(void);

#endif
