// Copyright 2018 Google LLC.
// Libgsound version: 5a34367
#ifndef GSOUND_TARGET_OTA_H
#define GSOUND_TARGET_OTA_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This header file was generated by Google GSound Services, it should not be
 * altered in any way. Please treat this as a read-only file.
 *
 * Each function listed below must be implemented for each specific platform/SDK
 *
 */
#include "gsound.h"

/**
 * OTA Description:
 *
 * The OTA process will follow the steps below
 * 1)   Init    <-- Called at boot-up
 *      ...
 * 2)   Get Status  <-- GSound will verify the OTA status of the Target
 * 3)   Begin   <-- Prepare start of new OTA process
 * 4)   Data    <-- Data received containing new firmware image fragment
 *      ...
 * n)   Data    <-- More Data
 *      ...
 * N)   Data    <-- Last firmware Data fragment
 * N+1) Apply   <-- Reboot into new firmware image
 *
 */

/**
 * Max length in bytes used by the OTA version string.  The OTA version string
 * include the terminating NULL character cannot exceed this value.
 */
#define GSOUND_TARGET_OTA_VERSION_MAX_LEN 16

/**
 * The maximum number of devices in the Target system which require a
 * unique or explicit OTA upgrade that GSound will support.
 */
#define GSOUND_TARGET_OTA_MAX_DEVICE_COUNT 2

/*******************************************************************************
 * Following declares the various state options available to request to resume
 * a previously interrupted OTA session.
 ******************************************************************************/
typedef enum {
  /**
   * OTA state in case no previous OTA session was active.
   */
  GSOUND_TARGET_OTA_RESUME_NONE,

  /**
   * OTA state in case a previous OTA session was active and Target has not yet
   * received all data required to apply the new OTA. Target should expect to
   * receive additional OTA Data packets if requesting to resume from this
   * state.
   */
  GSOUND_TARGET_OTA_RESUME_IN_PROGRESS,

  /**
   * OTA state in case a previous OTA session was active and Target has received
   * all data required to apply the new OTA. Target should expect to receive an
   * OTA Apply if requesting to resume from this state.
   */
  GSOUND_TARGET_OTA_RESUME_COMPLETE,

} GSoundOtaResumeState;

/*******************************************************************************
 * Following declares the various types of OTA devices
 ******************************************************************************/
typedef enum {
  /**
   * Full firmware.
   */
  GSOUND_TARGET_OTA_DEVICE_INDEX_FULL_FIRMWARE = 0,

  /**
   * Hotword models.
   */
  GSOUND_TARGET_OTA_DEVICE_INDEX_HOTWORD_MODEL = 1,
} GSoundOtaDeviceIndex;

/*******************************************************************************
 * Following declares the settings for an upcoming
 * OTA session which is about to begin.
 ******************************************************************************/
typedef struct {
  /**
   * Identifies the start offset, in bytes, from the start of the binary OTA
   * file for the new OTA session. Expect the byte_offset parameter of the first
   * GSoundTargetOtaData to match this value.
   *
   * Normally, when starting fresh, this value will be set to 0.
   *
   * This value can be >0 if Target has informed GSound to resume a previous OTA
   * session. However, a resume is not always possible - Target must always
   * honor the start_offset in the GSoundTargetOtaBegin call.  Refer to the
   * resume_offset member of GSoundOtaSessionSettings for more details.
   */
  uint32_t start_offset;

  /**
   * Total size in bytes the target should expect to receive for the next OTA
   * session. Target must use this value to track when it has received the full
   * firmware image
   */
  int32_t total_size;

  /**
   * Pointer to a NULL terminated ASCII string containing
   *
   * If device_index==FULL_FIRMWARE: version number
   *                ==HOTWORD_MODEL: model identifier
   *
   * Note: Target must make a copy of this string if required to use later. If
   * the device index corresponds to model update, the Target should save this
   * field and use it to map to the location of the model in NV storage.
   *
   * The string will be no larger than GSOUND_TARGET_OTA_VERSION_MAX_LEN
   */
  const char *ota_version;

  /**
   * Used by Target to identify which device in their system the new OTA session
   * is targeting.
   */
  GSoundOtaDeviceIndex device_index;
} GSoundOtaSessionSettings;

/*******************************************************************************
 * Following declares the Target's current OTA status.
 * This structure is used when GSound queries for OTA state information.
 * Refer to GSoundTargetOtaGetStatus for more details.
 ******************************************************************************/
typedef struct {
  /**
   * Identifies whether or not the Target is currently receiving an OTA from a
   * source other than GSound.
   */
  bool is_receiving_other;

} GSoundOtaStatus;

/*******************************************************************************
 * Following declares optional information Target must provide to GSound
 * in order to resume a previously interrupted OTA session.
 ******************************************************************************/
typedef struct {
  /**
   * The requested state of a previously interrupted OTA session.
   *
   * Target can only resume a previously interrupted OTA session if it has not
   * been aborted by GSoundTargetOtaAbort.
   *
   * For example, if the user powers off the device in the middle of an OTA
   * session, Target can request to resume at power on.
   */
  GSoundOtaResumeState requested_state;

  /**
   * The offset, in bytes, from the start of the OTA binary to resume the next
   * OTA. Target is responsible for storing this information in non-volatile
   * memory in case of power loss.
   *
   * Note: offset should be left unchanged if requested_state is set to
   * GSOUND_TARGET_OTA_RESUME_NONE.
   */
  uint32_t offset;

  /**
   * The version string of previously interrupted OTA session to resume. Target
   * must store the null terminated string at the location pointed to by this
   * variable.
   *
   * Note: version should be left unchanged if requested_state is
   * GSOUND_TARGET_OTA_RESUME_NONE.
   *
   * Note: Target should not write more than GSOUND_TARGET_OTA_VERSION_MAX_LEN
   * number of bytes including the NULL at this location to avoid memory
   * corruption.
   */
  char *const version;

  /**
   * The device index for the interrupted OTA session.
   */
  GSoundOtaDeviceIndex device_index;

} GSoundOtaResumeInfo;

/*******************************************************************************
 * Following declares detailed information Target can provide in response to
 * a GSoundTargetOtaData call.
 ******************************************************************************/
typedef struct {
  /**
   * Set this value to True only if the last OTA data packet has been fully
   * consumed by Target and Target is ready to receive an OTA Apply. Otherwise
   * set to False.
   *
   * If the last OTA Data packet is consumed asynchronously, i.e. after
   * returning from GSoundTargetOtaData, set this value to False and use the
   * callback gsound_target_on_ota_done to indicate OTA done.
   */
  bool ota_done;

  /* Optional output used by Target to specify the byte offset which OTA should
   * rewind to. Target can use this value instead of gsound_target_on_rewind to
   * request a rewind on returning from GSoundTargetOtaData. If a rewind is not
   * required, leave value unchanged. Refer to gsound_target_on_rewind() for
   * more details.
   */
  uint32_t rewind_offset;

} GSoundOtaDataResult;

/*******************************************************************************
 * Target-Specific OTA Configuration Settings.
 * Contains settings which may be unique for each Target
 * with regard to OTA.
 ******************************************************************************/
typedef struct {
  /**
   * Specifies the number of devices in the Target system which require a unique
   * or explicit OTA upgrade.
   *    - Non-Hotword: This value should be set to 1.
   *    - Hotword Model OTA supported: This value should be set to 2.
   */
  uint32_t ota_device_count;

  /**
   * Specifies whether the Target will apply an OTA automatically at the
   * Target's own discrection. If the OTA is a zero-day OTA, the Target must
   * immediately apply the OTA regardless of this setting.
   *
   * When set to false, the Target is expected to apply the OTA update
   * immediately when the GSoundTargetOtaApply function is called.
   *
   * When set to true, the GSoundTargetOtaApply function will be called
   * immediately after receiving all OTA data. The Target is then responsible to
   * apply the OTA update when possible and least inconvenient to the user (when
   * in the charging case, for instance).
   */
  bool supports_automatic_ota;
} GSoundOtaConfiguration;

/*******************************************************************************
 * Following declares the standard OTA
 * Interface API.
 ******************************************************************************/
typedef struct {
  /**
   * Callback used by Target to asynchronously indicate an error has occurred
   * during the OTA process. A call to this API will result in the OTA Begin
   * aborted and restarted later.
   *
   * Note: This callback cannot be executed from inside a Target-API OTA
   * function listed in this header.
   *
   * param[in]: status_code   Status code indicating the error
   */
  GSoundStatus (*gsound_target_on_ota_error)(GSoundStatus status_code);

  /**
   * Callback used by Target to asynchronously request the OTA process to rewind
   * back to an earlier byte offset. This API should be called any time the
   * Target had to discard some amount of OTA data and requires retransmission.
   * For example this API should be used if the Target cannot keep up with the
   * OTA due to resource/throughput constraints and some packets were dropped.
   *
   * As a result of this API, Target should expect a subsequent call to
   * GSoundTargetOtaData(...) containing the offset specified by resume_offset.
   *
   * Note: It is possible that GSound will call GSoundTargetOtaData(...) with an
   * unexpected byte_offset after this API is called. If Target receives an
   * incorrect byte_offset, it should silently drop that packet. The correct
   * byte offset will eventually arrive.
   *
   * Note: This callback cannot be executed from inside a Target-API OTA
   * function listed in this header.
   *
   * param[in]: rewind_offset   The byte offset which OTA should rewind to.
   *
   */
  GSoundStatus (*gsound_target_on_rewind)(uint32_t rewind_offset);

  /**
   * Callback used by Target to asynchronously indicate that the last OTA data
   * packet has been fully consumed by the device and Target is ready to receive
   * an OTA Apply.
   *
   * Note: This callback is optional. If Target consumes the last OTA Data
   * packet synchronously from within a call to GSoundTargetOtaData, this
   * callback is not required. In this case, Target should use the ota_done
   * field inside GSoundOtaDataResult.
   */
  GSoundStatus (*gsound_target_on_ota_done)(void);

} GSoundOtaInterface;

/*******************************************************************************
 * Function declarations
 ******************************************************************************/

/**
 * Called at the start of a new OTA process when end user has requested to
 * upgrade firmware.
 *
 * Note: In case of a TWS system with ota_device_count set to 1, this API should
 * also ensure Slave device is aware of a new OTA process
 *
 * Note: In case of TWS system, if slave is not present, Target should not Begin
 * full firmware OTA and instead return an error. However, model OTA must be
 * allowed even if the slave is not present.
 *
 * Note: Target must ignore this API and return an error if receiving an OTA
 * from a source other than GSound.
 *
 * param[in]: ota_settings Pointer to settings structure. refer to
 *                         GSoundOtaSessionSettings documentation for more
 *                         details. Target must fully consume the contents
 *                         of this structure before returning.
 */
GSoundStatus GSoundTargetOtaBegin(const GSoundOtaSessionSettings *ota_settings);

/**
 * OTA data has arrived containing a fragment of the firmware image. This
 * function will be called multiple times, once for every fragment of the
 * firmware image. It will be called until all fragments have been transmitted
 * from remote device.
 *
 * Note: Target is responsible for internally tracking the remaining number of
 * bytes. The total number of bytes Target should expect is provided by the
 * GSoundTargetOtaBegin function call.
 *
 * Every time this function is called, Target must ensure data is either
 * buffered in preparation to be flushed to flash and/or store to flash. In any
 * case, the data provided by this function must be fully consumed before
 * returning from this function.
 *
 * Target must identify that the last OTA fragment has been received by this
 * function. When that happens, target must make sure any partially filled
 * buffer is flushed to flash and finally verify the integrity of the final
 * image. If the image is invalid for any reason, Target should return an error.
 *
 * Target is also responsible for ensuring the wrong binary was not sent by
 * verifying the final image supports the model of the current device.
 *
 * Note: In case of TWS system, this API should also ensure Slave device
 * receives new firmware data fragment if required. Target should use the
 * device_index provided in the last call to GSoundTargetOtaBegin to route data
 * to correct device in system.
 *
 * Note: In case of TWS system during full firmware OTA, if slave is not
 * present, or a role change occurs, Target should ignore this data and instead
 * return an error. Target must also reset its internal environment and prepare
 * itself for a new OTA session.
 *
 * Note: Target must ignore this API and return an error if receiving an OTA
 * from a source other than GSound.
 *
 * param[in]:  data         Pointer to OTA data. Target must fully consume this
 *                          data before returning
 * param[in]:  length       Length in bytes of data
 * param[in]:  byte_offset  The number of bytes which has already been
 *                          transferred.
 * param[out]: ota_status   Pointer to GSoundOtaDataResult which Target should
 *                          populate with status information. Refer to
 *                          GSoundOtaDataResult for more information.
 * param[in]:  device_index  Used by Target to route Data to the correct device
                             in the system.
 */
GSoundStatus GSoundTargetOtaData(const uint8_t *data, uint32_t length,
                                 uint32_t byte_offset,
                                 GSoundOtaDataResult *ota_status,
                                 GSoundOtaDeviceIndex device_index);

/**
 * At this point, all data has been transferred and we are ready to apply the
 * new image
 *
 * For full firmware Target should reboot to use the new image. If
 * supports_automatic_ota is false or if is_zero_day_ota is true, we should not
 * return from this function.
 *
 * For hotword model OTA Target should not reboot.
 *
 * On system Bootup, the platform bootloader should:
 *    1) Ensure new firmware is available
 *    2) Verify the integrity of the new firmware
 *    3) Copy the new firmware to final location where it will execute from
 *    4) Boot up new firmware.
 *
 * Note: In case of TWS system with ota_device_count set to 1, this API should
 * also ensure Slave device reboots into new firmware image following the steps
 * above.
 *
 * Note: In case of TWS system during full firmware OTA, if slave is not
 * present, or a role change occurs, Target should ignore the Apply and instead
 * return an error. Target must also reset its internal environment and prepare
 * itself for a new OTA session.
 *
 * Note: Target must ignore this API and return an error if receiving an OTA
 * from a source other than GSound.
 *
 * param[in]: device_index    Used by Target to route Apply to the correct
 *                            device in the system.
 * param[in]: is_zero_day_ota Indicates whether this is a zero-day OTA. If
 *                            true, the OTA should be applied immediately.
 *
 */
GSoundStatus GSoundTargetOtaApply(GSoundOtaDeviceIndex device_index,
                                  bool is_zero_day_ota);

/**
 * Function called when OTA is canceled.
 *
 * Any previously transmitted OTA data can no longer be used. Target must reset
 * its internal environment and prepare itself for a new OTA session, starting
 * with GSoundTargetOtaBegin() with offset of 0.
 *
 * Target must also reset any stored resume information when aborted.
 *
 * Note: In case of TWS system with ota_device_count set to 1, this API should
 * also ensure Slave device handles the OTA abort.
 *
 * Note: Target must ignore this API and return an error if receiving an OTA
 * from a source other than GSound.
 *
 * param[in]: device_index  Used by Target to route Abort to the correct device
 *                          in the system.
 *
 * Returns GSOUND_STATUS_OK
 */
GSoundStatus GSoundTargetOtaAbort(GSoundOtaDeviceIndex device_index);

/**
 * Function called to fetch the current OTA status of the device.
 *
 * param[out]: target_ota_status Pointer to a data structure which
 *                               Target must populate with its current OTA
 *                               status. Refer to GSoundOtaStatus for
 *                               details.
 * param[in]: device_index:      Identifies which device in the Target system
 *                               GSound is requesting status from.
 */
void GSoundTargetOtaGetStatus(GSoundOtaStatus *target_ota_status,
                              GSoundOtaDeviceIndex device_index);

/**
 * Function called to fetch the OTA resume information from Target.
 *
 * Target is responsible for storing resuming information in non-volatile memory
 * in case of power loss.
 *
 * Note, a resume is not always possible. If Target requests to resume an OTA
 * they can still receive a GSoundTargetOtaBegin with offset = 0. In this case
 * OTA must start over.
 *
 * param[out]: target_resume_info Pointer to a data structure which
 *                                Target must populate with its OTA resume
 *                                information. Refer to GSoundOtaResumeInfo for
 *                                details.
 */
void GSoundTargetOtaGetResumeInfo(GSoundOtaResumeInfo *target_resume_info);

/**
 * Called at boot-up.
 *
 * Target can use this function call to initialize any internal variables
 * required to process a future OTA.
 *
 * param[in]:  handler           Pointer to OTA handlers.
 * param[out]: ota_device_config Pointer to an output structure which Target
 *                               should populate with the OTA configuration
 *                               settings. Refer to GSoundOtaConfiguration
 *                               type for more details.
 *
 * Returns GSOUND_STATUS_OK    In case of no errors and Target is ready to
 *                             receive a subsequent OTA from GSound
 *         GSOUND_STATUS_ERROR In case of an error which will prevent future
 *                             OTA or if the Target does not support OTA from
 *                             GSound.
 */
GSoundStatus GSoundTargetOtaInit(const GSoundOtaInterface *handler,
                                 GSoundOtaConfiguration *ota_device_config);
#ifdef __cplusplus
}
#endif

#endif  // GSOUND_TARGET_OTA_H
