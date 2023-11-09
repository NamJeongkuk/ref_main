/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOARDFEATUREAPI_H
#define BOARDFEATUREAPI_H

#include <stdint.h>

typedef uint32_t BoardFeatureWord_t;

typedef uint8_t BoardFeatureApiVersion_t;

enum
{
   BoardFeatureApiBoard_DispenserUi
};
typedef uint8_t BoardFeatureApiBoard_t;

typedef struct
{
   BoardFeatureApiVersion_t featureApiVersion;
   BoardFeatureApiBoard_t featureApiType;
   BoardFeatureWord_t featureBits;
} BoardFeatureApi_t;

#define SWAP_DEFINITION_BoardFeatureApi_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, featureBits),

#endif
