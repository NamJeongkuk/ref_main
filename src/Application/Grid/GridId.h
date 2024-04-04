/*!
 * @file
 * @brief Grid Id type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRIDID_H
#define GRIDID_H

enum
{
   GridId_SingleEvap,
   GridId_DualEvap,
   GridId_TripleEvap,
   GridId_SingleDoorSingleEvap,
   GridId_FeaturePan,
   GridId_IceCabinet
};
typedef uint8_t GridId_t;

#endif
