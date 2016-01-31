#include <stdio.h>
#include <string.h>
#include "common_include.h"
#include "calendar_manager.h"
#include "rtos.h"
#include "types.h"

int32_t calendar_exit = CALENDAR_RUNNING;

static void *calendar_manager_thread_entry(void *param)
{
  UNUSED(param);
  char8_t buffer[MAX_MSG_QUEUE_SIZE + 1];
  mqd_hdl mq;

  memset(buffer, '\0', MAX_MSG_QUEUE_SIZE+1);

  if(FAILURE == QueueCreate(&mq, QUEUE_NAME, MAX_MSG_QUEUE_SIZE, MAX_MSG_QUEUE_NUM))
  {
    calendar_quit();
  }

  while(!calendar_exit)
  {
    if(FAILURE == QueueReceive(&mq, buffer, MODE_BLOCK))
    {
      calendar_quit();
    }

    CALENDER_DEBUG("Success to receive message : [ %s ] from user input process thread.", buffer);

    if(FAILURE == QueueSend(&mq, "Calendar Manager has received message...processing", MODE_BLOCK))
    {
      calendar_quit();
    }

    CALENDER_DEBUG("Success to Answer message to user input process thread.");
  }

  QueueDelete(&mq, QUEUE_NAME);
}

void calendar_manager_thread_init(void)
{
  thread_hdl thread_id;
  Bool ret;

  ret = ThreadCreate( &thread_id,
                                 THREAD_CALENDAR_MANAGER_NAME,
                                 THREAD_DEFAULT_PRIORITY,
                                  NULL,
                                  THREAD_DEFAULT_STACK_SIZE,
                                  calendar_manager_thread_entry,
                                  (void *)(NULL) );

  if(FAILURE == ret)
  {
    calendar_quit();
  }

  if(FAILURE == ThreadDetach(&thread_id))
  {
    calendar_quit();
  }

}

