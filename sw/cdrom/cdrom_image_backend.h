/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          CD-ROM image file handling module header , translated to C
 *          from cdrom_dosbox.h.
 *
 * Authors: Miran Grca, <mgrca8@gmail.com>
 *          Fred N. van Kempen, <decwiz@yahoo.com>
 *          The DOSBox Team, <unknown>
 *          Kevin Moonlight
 *          Ian Scott
 *
 *          Copyright 2016-2020 Miran Grca.
 *          Copyright 2017-2020 Fred N. van Kempen.
 *          Copyright 2002-2020 The DOSBox Team.
 *          Copyright (C) 2024 Kevin Moonlight
 *          Copyright (C) 2025 Ian Scott
 */

#ifndef CDROM_IMAGE_BACKEND_H
#define CDROM_IMAGE_BACKEND_H

#include <stdint.h>
#include <stddef.h>

#include "ff.h"

#define RAW_SECTOR_SIZE    2352
#define COOKED_SECTOR_SIZE 2048

#define DATA_TRACK         0x14
#define AUDIO_TRACK        0x10

#define CD_FPS             75
#define CD_FPM             4500
/*
#define FRAMES_TO_MSF(f, M, S, F)                 \
    {                                             \
        uint64_t value = f;                       \
        *(F)           = (value % CD_FPS) & 0xff; \
        value /= CD_FPS;                          \
        *(S) = (value % 60) & 0xff;               \
        value /= 60;                              \
        *(M) = value & 0xff;                      \
    }
*/
#define MSF_TO_FRAMES(M, S, F) ((M) * 60 * CD_FPS + (S) * CD_FPS + (F))

typedef struct SMSF {
    uint8_t min;
    uint8_t  sec;
    uint8_t  fr;
} TMSF;

#define SZ_TBL 32

/* Track file struct. */
typedef struct track_file_t {
    int (*read)(void *priv, uint8_t *buffer, uint32_t seek, size_t count);
    // uint64_t (*get_length)(void *priv);
    uint32_t (*get_length)(void *priv);
    void (*close)(void *priv);

    char  fn[128];
    FIL *fp;
    void *priv;
    // fast seek cluster link map table
    uint32_t clmt[SZ_TBL];
} track_file_t;

typedef struct track_t {
    int           number;
    int           track_number;
    int           attr;
    int           sector_size;
    int           mode2;
    int           form;
    int           pad;
    // uint64_t      start;
    uint32_t      start;
    // uint64_t      length;
    uint32_t      length;
    // uint64_t      skip;
    uint32_t      skip;
    track_file_t *file;
} track_t;

typedef struct cd_img_t {
    int      tracks_num;
    track_t *tracks;
} cd_img_t;

/* Binary file functions. */
extern void cdi_close(cd_img_t *cdi);
extern int  cdi_set_device(cd_img_t *cdi, const char *path);
extern void cdi_get_audio_tracks(cd_img_t *cdi, int *st_track, int *end, TMSF *lead_out);
extern void cdi_get_audio_tracks_lba(cd_img_t *cdi, int *st_track, int *end, uint32_t *lead_out);
extern int  cdi_get_audio_track_info(cd_img_t *cdi, int end, int track, int *track_num, TMSF *start, uint8_t *attr);
extern int  cdi_get_audio_track_info_lba(cd_img_t *cdi, int end, int track, int *track_num, uint32_t *start, uint8_t *attr);
extern int  cdi_get_track(cd_img_t *cdi, uint32_t sector);
extern int  cdi_get_audio_sub(cd_img_t *cdi, uint32_t sector, uint8_t *attr, uint8_t *track, uint8_t *index, TMSF *rel_pos, TMSF *abs_pos);
extern int  cdi_read_sector(cd_img_t *cdi, uint8_t *buffer, int raw, uint32_t sector);
extern int  cdi_read_sectors(cd_img_t *cdi, uint8_t *buffer, int raw, uint32_t sector, uint32_t num);
extern int  cdi_read_sector_sub(cd_img_t *cdi, uint8_t *buffer, uint32_t sector);
extern int  cdi_get_sector_size(cd_img_t *cdi, uint32_t sector);
extern int  cdi_is_mode2(cd_img_t *cdi, uint32_t sector);
extern int  cdi_get_mode2_form(cd_img_t *cdi, uint32_t sector);
extern int  cdi_load_iso(cd_img_t *cdi, const char *filename);
extern int  cdi_load_cue(cd_img_t *cdi, const char *cuefile);
extern int  cdi_has_data_track(cd_img_t *cdi);
extern int  cdi_has_audio_track(cd_img_t *cdi);

#endif /*CDROM_IMAGE_BACKEND_H*/
