// Copyright 2019 Google LLC.
// Libgsound version: 5a34367
#ifndef GSOUND_TARGET_HOTWORD_MODEL_COMMON_H
#define GSOUND_TARGET_HOTWORD_MODEL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * This file is used for all targets implementing hotword, where the hotword
 * detection is handled by GSound (internal) or by the target (external).
 */

#define SUPPORTED_HOTWORD_MODEL_DELIM "\n"

#define GSOUND_HOTWORD_MODEL_ID_BYTES 4
#define SUPPORTED_HOTWORD_MODEL_DELIM_BYTES 1
#define GSOUND_MAX_SUPPORTED_MODELS 8
#define GSOUND_HOTWORD_SUPPORTED_MODEL_ID_BYTES \
  ((GSOUND_HOTWORD_MODEL_ID_BYTES) + (SUPPORTED_HOTWORD_MODEL_DELIM_BYTES))
#define GSOUND_MAX_SUPPORTED_HOTWORD_MODELS_BYTES \
  ((GSOUND_MAX_SUPPORTED_MODELS) * (GSOUND_HOTWORD_SUPPORTED_MODEL_ID_BYTES))

typedef enum {
  /**
   * An mmap of this type is Read-Only data
   */
  GSOUND_HOTWORD_MMAP_TEXT,
  /*
   * An mmap of this type is preinitialized Read-Write data
   */
  GSOUND_HOTWORD_MMAP_DATA,
  /*
   * An mmap of this type is uninitialized Read-Write data
   */
  GSOUND_HOTWORD_MMAP_BSS
} GSoundHotwordMmapType;

#ifdef __cplusplus
}
#endif

#endif  // GSOUND_TARGET_HOTWORD_MODEL_COMMON_H
