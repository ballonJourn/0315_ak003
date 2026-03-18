/*
 * rear_camera_detect_task.h
 *
 *  Created on: 2024年4月29日
 *      Author: pengzc
 */

#ifndef MISC_REAR_VIEW_DETECTION_H_
#define MISC_REAR_VIEW_DETECTION_H_

#include <mpi/common.h>

class RearViewDetection: public mpi::Task<> {
public:
  virtual ~RearViewDetection();

  DISALLOW_COPY_AND_ASSIGN_METHOD(RearViewDetection);
  DECLARE_SINGLETON_PATTERN(RearViewDetection);

protected:
  virtual void doTask() override;
};

#endif /* MISC_REAR_VIEW_DETECTION_H_ */
