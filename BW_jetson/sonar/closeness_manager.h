#ifndef CLOSENESS_MANAGER_H
#define CLOSENESS_MANAGER_H

#define NUM_SONAR 7
#define HISTORY_SIZE 3          // to evaluate decrease in time
#define APPROACH_THRESHOLD 50.0  // collision risk threshold

#define VIDEO_FILE "../vision/camera_detection.txt"
#define CLOSENESS_FILE "../sonar/closeness_detection.txt"

void init_sonar(void); 
void closeness_manager(const char *buf);

#endif
