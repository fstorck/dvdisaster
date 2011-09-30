/*  dvdisaster: Additional error correction for optical media.
 *  Copyright (C) 2004-2010 Carsten Gnoerlich.
 *  Project home page: http://www.dvdisaster.com
 *  Email: carsten@dvdisaster.com  -or-  cgnoerlich@fsfe.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA,
 *  or direct your browser at http://www.gnu.org.
 */

#ifndef UDF_H
#define UDF_H

#include "scsi-layer.h"

/*
 * Structure and functions for examining an existing .iso image 
 */

typedef struct _IsoInfo
{  guint32 volumeSize;
   char volumeLabel[33];
   char creationDate[32];
} IsoInfo;

void FreeIsoInfo(IsoInfo*);

EccHeader* FindHeaderInMedium(DeviceHandle*, gint64);
gint64 MediumLengthFromRS02(DeviceHandle*, gint64);

int ExamineUDF(DeviceHandle*);

/*
 * Structure and functions for creating an .iso image
 */

typedef struct _IsoDir
{  unsigned char *dir;   /* memory allocated for dir, multiples of 2048 */
   int nSectors;         /* number of sectors */
   int tail;             /* next place for inserting new entry */
} IsoDir;

typedef struct _IsoPathTable
{  unsigned char *lpath; /* memory allocated for L path table, multiples of 2048 */
   unsigned char *mpath; /* memory allocated for R path table, multiples of 2048 */
   int nSectors;         /* number of sectors */
   int tail;             /* next place for inserting new entry */
} IsoPathTable;

typedef struct _IsoHeader
{  unsigned char *pvd;   /* primary volume descriptor */
   IsoDir *proot;        /* its root directory */
   IsoPathTable *ppath;  /* and path table */
   unsigned char *svd;   /* supplementary volume descriptor */
   IsoDir *sroot;        /* its root directory */
   IsoPathTable *spath;  /* and path table */
   int dirSectors;
   guint64 volumeSpace;
} IsoHeader;

IsoHeader* InitIsoHeader(void);
void AddFile(IsoHeader*, char*, guint64);
void FreeIsoHeader(IsoHeader*);
void WriteIsoHeader(IsoHeader*, LargeFile*);

#endif
